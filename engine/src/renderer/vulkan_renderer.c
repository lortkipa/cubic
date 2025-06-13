#include "renderer/vulkan_renderer.h"
#include "platform/vulkan_platform.h"
#include "platform/window.h"
#include "core/memory.h"
#include "core/stack_allocator.h"
#include "core/logger.h"
#include <string.h>
#include <stdio.h>

#define CHANNEL "Vulkan Renderer"

static StackAllocator allocator;

static VKRenderer* renderer;

static VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
{
    // log with verbosity depending on callback severty
    switch (messageSeverity) 
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LogError("Vulkan Messenger", "%s", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LogWarning("Vulkan Messenger", "%s", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LogInfo("Vulkan Messenger", "%s", pCallbackData->pMessage);
            break;
        default:
            LogWarning("Vulkan Messenger", "Unknow Severity Provided");
            break;
    }

    return VK_FALSE;
}

static VkResult CreateVKMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* p_info, const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_messenger)
{
    // load function
    auto function = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (function)
    {
        return function(instance, p_info, p_allocator, p_messenger);
    }
    else 
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void DestroyVKMessenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* p_allocator) {
    auto function = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (function)
    {
        function(instance, messenger, p_allocator);
    }
}


b8 StartupVKRenderer(void)
{
    // create allocator
    if (!CreateStackAllocator(&allocator, sizeof(VKRenderer) + 3000))
    {
        LogError(CHANNEL, "Stack Allocator Creation Failed");
        return false;
    }

    // request heap memory for renderer
    renderer = RequestStackAllocatorMemory(&allocator, sizeof(VKRenderer));

    // create instance
    {
        // get instance extensions
        u8 extCount;
#if defined(DEBUG)
        const char** exts = GetRequiredVKInstanceExtensions(&extCount, true);
#else
        const char** exts = GetRequiredVKInstanceExtensions(&extCount, false);
#endif

        // log extensions in debug mode
#if defined(DEBUG)

        for (u8 i = 0; i < extCount; i++)
        {
            LogInfo(CHANNEL, "Loading Extension: %s", exts[i]);
        }

#endif

        // validation layers
        const char* layers[] = 
        {
            "VK_LAYER_KHRONOS_validation"
        };

        // calculate validation layer count
        u8 layerCount = sizeof(layers) / sizeof(char*);

        // log validation layers in debug mode
#if defined(DEBUG)

        for (u8 i = 0; i < layerCount; i++)
        {
            LogInfo(CHANNEL, "Loading Validation Layer: %s", layers[i]);
        }

#endif

        // app info
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Cubic Engine";
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // instance info
        VkInstanceCreateInfo instanceInfo = {};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = extCount;
        instanceInfo.ppEnabledExtensionNames = exts;

        // enable validation layers in debug mode only
#if defined(DEBUG)

        instanceInfo.enabledLayerCount = layerCount;
        instanceInfo.ppEnabledLayerNames = layers;

#endif

        // create instance
        VkResult result = vkCreateInstance(&instanceInfo, null, &renderer->instance);

        // check for errors
        if (result != VK_SUCCESS)
        {
            LogError(CHANNEL, "Instance Creation Failed");
            return false;
        }
        LogSuccess(CHANNEL, "Instance Created");
    }

    // create messenger if app is in debug mode
#if defined(DEBUG)

    {
        // messenger create info
        VkDebugUtilsMessengerCreateInfoEXT messengerInfo = {};
        messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        messengerInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        messengerInfo.messageType = 
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        messengerInfo.pfnUserCallback = VKDebugCallback;

        // create messenger
        VkResult result = CreateVKMessenger(renderer->instance, &messengerInfo, null, &renderer->messenger);

        // check for errors
        if (result != VK_SUCCESS)
        {
            LogWarning(CHANNEL, "Messenger Not Created");
        }
        LogSuccess(CHANNEL, "Messenger Created");
    }

#endif

    // create surface
    {
        // create it
        VkResult result = CreateVKSurface(renderer);

        // check for errors
        if (result != VK_SUCCESS)
        {
            LogError(CHANNEL, "Surface Creation Failed");
            return false;
        }
        LogSuccess(CHANNEL, "Surface Created");
    }

    // physical device extensions
    const char* physicalDeviceExts[] = 
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // calculate physical device extension count
    u8 physicalDeviceExtCount = sizeof(physicalDeviceExts) / sizeof(char*);

    // pick physical device (GPU)
    {
        // get physical device count
        u32 physicalDeviceCount;
        vkEnumeratePhysicalDevices(renderer->instance, &physicalDeviceCount, null);

        // make sure at list 1 gpu is found
        if (physicalDeviceCount == 0)
        {
            LogError(CHANNEL, "Failed To Find GPU With Vulkan Support");
            return false;
        }

        // get physical devices
        VkPhysicalDevice* physicalDevices = RequestStackAllocatorMemory(&allocator, physicalDeviceCount * sizeof(VkPhysicalDevice));
        vkEnumeratePhysicalDevices(renderer->instance, &physicalDeviceCount, physicalDevices);

        // in debug mode, log every physical device info
#if defined(DEBUG)

        for (u32 i = 0; i < physicalDeviceCount; i++)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);
            LogInfo(CHANNEL, "GPU Found: \"%s\"", properties.deviceName);
        }

#endif

        // loop thro physical devices to pick one of them
        renderer->physicalDevice = null;
        for (u32 i = 0; i < physicalDeviceCount; i++)
        {
            // query gpu info
            VkPhysicalDeviceProperties properties;
            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);
            vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);

            // get physical device queue family count
            u32 queueFamilyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, null);

            // get physical device queue families
            VkQueueFamilyProperties* queueFamilies = RequestStackAllocatorMemory(&allocator, queueFamilyCount * sizeof(VkQueueFamilyProperties));
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, queueFamilies);

            // track if queue families are found
            b8 graphicsQueueFamilyFound = false;
            b8 presentQueueFamilyFound = false;

            // loop thro queue families
            for (u32 j = 0; j < queueFamilyCount; j++)
            {
                // try to find graphics family index
                if (!graphicsQueueFamilyFound && (queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT))
                {
                    graphicsQueueFamilyFound = true;
                    LogInfo(CHANNEL, "Graphics Queue Family Found At Index: %d", j);
                }

                // try to find present family index
                if (!presentQueueFamilyFound)
                {
                    // get present family queue support
                    VkBool32 presentSupport = false;
                    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[i], i, renderer->surface, &presentSupport);

                    // if present family is supported, store its index
                    if (presentSupport)
                    {
                        presentQueueFamilyFound = true;
                        renderer->queueFamilyIndinces.present = i;
                    }
                }

                // save family indinces and exit the loop, if all indinces are found
                if (graphicsQueueFamilyFound && presentQueueFamilyFound)
                {
                    renderer->queueFamilyIndinces.graphics = i;
                    break;
                }
            }

            // free queue families from heap
            RequestStackAllocatorMemory(&allocator, queueFamilyCount * sizeof(VkQueueFamilyProperties));

            // get awailable gpu extension count
            u32 awailableExtCount;
            vkEnumerateDeviceExtensionProperties(physicalDevices[i], null, &awailableExtCount, null);

            // get awailable gpu extensions
            VkExtensionProperties awailableExts[awailableExtCount];
            vkEnumerateDeviceExtensionProperties(physicalDevices[i], null, &awailableExtCount, null);

            // track wich needed extension was found
            b8 extStatuses[awailableExtCount] = {};

            // loop needed extension
            for (u8 i = 0; i < physicalDeviceExtCount; i++)
            {
                // loop awailable extensions
                for (u32 j = 0; j < awailableExtCount; j++)
                {
                    // if extensions is found track it
                    if (strcmp(physicalDeviceExts[i], awailableExts[j].extensionName))
                    {
                        extStatuses[i] = true;
                        LogInfo(CHANNEL, "Extension Loaded: \"%s\"", physicalDeviceExts[i]);
                        break;
                    }
                }
            }

            // track if all needed extension is supported
            b8 extsSupported = false;

            // check if all extension is found
            for (u8 i = 0; i < physicalDeviceExtCount; i++)
            {
                if (!extStatuses[i])
                {
                    break;
                }

                if (i == (physicalDeviceExtCount - 1) && extStatuses[i])
                {
                    LogInfo(CHANNEL, "All Physical Device Extension Are Supported (%d)", physicalDeviceExtCount);
                    extsSupported = true;
                }
            }

            // get format count
            u32 formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[i], renderer->surface, &formatCount, null);
            LogInfo(CHANNEL, "Surface Format Found: %d", formatCount);

            // get present mode count
            u32 presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[i], renderer->surface, &presentModeCount, null);
            LogInfo(CHANNEL, "Surface Present Mode Found: %d", presentModeCount);

            // track if swapchain is supported for this gpu
            b8 swapchainIsSupported = false;

            // make sure swapchain has at list 1 supported present mode and 1 supported format
            if (formatCount > 0 && presentModeCount > 0)
            {
                swapchainIsSupported = true;
            }

            // try to pick gpu
            if (graphicsQueueFamilyFound && swapchainIsSupported && extsSupported && features.geometryShader)
            {
                LogSuccess(CHANNEL, "GPU Picked: \"%s\"", properties.deviceName);
                renderer->physicalDevice = physicalDevices[i];
                break;
            }
        }

        // release physical devices memory
        FreeStackAllocatorMemory(&allocator, physicalDeviceCount * sizeof(VkPhysicalDevice));

        // if physical device is not picked, return failure
        if (!renderer->physicalDevice)
        {
            LogError(CHANNEL, "Failed To Pick Suitable GPU");
            return 0;
        }

    }

    // create logical device
    {
        // logical device queue infos
        float queuePriority = 1.0f;
        u8 queueInfoCount = 
            (renderer->queueFamilyIndinces.graphics == (u32)renderer->queueFamilyIndinces.present) ? 1 : 2;
        VkDeviceQueueCreateInfo* queueInfos = RequestStackAllocatorMemory(&allocator, queueInfoCount * sizeof(VkDeviceQueueCreateInfo));
        for (u32 i = 0; i < queueInfoCount; i++)
        {
            queueInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfos[i].queueFamilyIndex =
                i == 0 ? renderer->queueFamilyIndinces.graphics : renderer->queueFamilyIndinces.present;
            queueInfos[i].queueCount = 1;
            queueInfos[i].pQueuePriorities = &queuePriority;
            queueInfos[i].pNext = null;
            queueInfos[i].flags = 0;
        }

        // set app features
        VkPhysicalDeviceFeatures rendererFeatures = {};

        // logical device info
        VkDeviceCreateInfo logicalDeviceInfo = {};
        logicalDeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        logicalDeviceInfo.pQueueCreateInfos = queueInfos;
        logicalDeviceInfo.queueCreateInfoCount = queueInfoCount;
        logicalDeviceInfo.pEnabledFeatures = &rendererFeatures;
        logicalDeviceInfo.ppEnabledExtensionNames = physicalDeviceExts;
        logicalDeviceInfo.enabledExtensionCount = physicalDeviceExtCount;

        // create logical device
        VkResult result = vkCreateDevice(renderer->physicalDevice, &logicalDeviceInfo, null, &renderer->logicalDevice);

        // free queue infos from heap
        FreeStackAllocatorMemory(&allocator, queueInfoCount * sizeof(VkDeviceQueueCreateInfo));

        // check for errors
        if (result != VK_SUCCESS)
        {
            LogError(CHANNEL, "Logical Device Not Created");
            return 0;
        }
        LogSuccess(CHANNEL, "Logical Device Created");
    }

    // save queue handles
    {
        // graphics queue
        vkGetDeviceQueue(renderer->logicalDevice, renderer->queueFamilyIndinces.graphics, 0, &renderer->queueHandles.graphics);

        // present queue
        vkGetDeviceQueue(renderer->logicalDevice, renderer->queueFamilyIndinces.present, 0, &renderer->queueHandles.present);
    }

    // create swapchain
    {
        // get format count
        u32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->physicalDevice, renderer->surface, &formatCount, null);
        LogInfo(CHANNEL, "Surface Format Found: %d", formatCount);

        // get formats
        VkSurfaceFormatKHR* formats = RequestStackAllocatorMemory(&allocator, formatCount * sizeof(VkSurfaceFormatKHR));
        vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->physicalDevice, renderer->surface, &formatCount, formats);

        // store picked format here
        VkSurfaceFormatKHR format;
        b8 formatIsChoosen = false;

        // loop formats
        for (u32 i = 0; i < formatCount; i++)
        {
            // try to pick best format
            if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
            {
                format = formats[i];
                formatIsChoosen = true;
                break;
            }
        }

        // if best format is not picked, choose first one
        if (!formatIsChoosen)
        {
            format = formats[0];
        }

        // store vkImageFormat
        renderer->imageFormat = format.format;

        // free formats array from heap
        FreeStackAllocatorMemory(&allocator, formatCount * sizeof(VkSurfaceFormatKHR));

        // get present mode count
        u32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->physicalDevice, renderer->surface, &presentModeCount, null);
        LogInfo(CHANNEL, "Surface Present Mode Found: %d", presentModeCount);

        // get present modes
        VkPresentModeKHR* presentModes = RequestStackAllocatorMemory(&allocator, presentModeCount * sizeof(VkPresentModeKHR));
        vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->physicalDevice, renderer->surface, &presentModeCount, presentModes);

        // store picked present mode here
        VkPresentModeKHR presentMode;
        b8 presentModeIsChoosen = false;

        // loop present modes
        for (u32 i = 0; i < presentModeCount; i++)
        {
            // try to pick best present mode
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                presentMode = presentModes[i];
                presentModeIsChoosen = true;
                LogInfo(CHANNEL, "Best Present Mode Choosen: \"VK_PRESENT_MODE_MAILBOX_KHR\"");
                break;
            }
        }

        // if best present mode is not awailable, choose guarentieed one
        if (!presentModeIsChoosen)
        {
            presentMode = VK_PRESENT_MODE_FIFO_KHR;
            LogInfo(CHANNEL, "Present Mode Choosen: \"VK_PRESENT_MODE_FIFO_KHR\"");
        }

        // free present modes array from heap
        FreeStackAllocatorMemory(&allocator, presentModeCount * sizeof(VkPresentModeKHR));

        // store surface capabilities here
        VkSurfaceCapabilitiesKHR capabilities;

        // get surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer->physicalDevice, renderer->surface, &capabilities);

        // choose extend2d settings
        if (capabilities.currentExtent.width != UINT32_MAX && capabilities.currentExtent.height != UINT32_MAX)
        {
            renderer->extent.width = capabilities.currentExtent.width;
            renderer->extent.height = capabilities.currentExtent.height;
        }
        else 
        {
            WindowSize windowSize = GetWindowSize();

            renderer->extent.width = windowSize.width;
            renderer->extent.height = windowSize.height;

            if (renderer->extent.width < capabilities.minImageExtent.width)
            {
                renderer->extent.width = capabilities.minImageExtent.width;
            } 
            else if (renderer->extent.width > capabilities.maxImageExtent.width)
            {
                renderer->extent.width = capabilities.maxImageExtent.width;
            }

            if (renderer->extent.height < capabilities.minImageExtent.height)
            {
                renderer->extent.height = capabilities.minImageExtent.height;
            } 
            else if (renderer->extent.height > capabilities.maxImageExtent.height)
            {
                renderer->extent.height = capabilities.maxImageExtent.height;
            }
        }
        LogInfo(CHANNEL, "Extent2D Choosen: { Width: %d, Height: %d }",
                renderer->extent.width, renderer->extent.height);

        // setup image count of swapchain queue
        u32 imageCount = capabilities.minImageCount + 1;

        // swapchain creation info
        VkSwapchainCreateInfoKHR swapchainInfo = {};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = renderer->surface;
        swapchainInfo.minImageCount = imageCount;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageFormat = format.format;
        swapchainInfo.imageColorSpace = format.colorSpace;
        swapchainInfo.imageExtent = renderer->extent;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (renderer->queueFamilyIndinces.graphics != renderer->queueFamilyIndinces.present)
        {
            u32 indinces[] = { renderer->queueFamilyIndinces.graphics != renderer->queueFamilyIndinces.present };
            swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainInfo.queueFamilyIndexCount = 2;
            swapchainInfo.pQueueFamilyIndices = indinces;
        }
        else 
        {
            swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        swapchainInfo.preTransform = capabilities.currentTransform;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.presentMode = presentMode;
        swapchainInfo.clipped = VK_TRUE;
        swapchainInfo.oldSwapchain = null;

        // create swapchain
        VkResult result = vkCreateSwapchainKHR(renderer->logicalDevice, &swapchainInfo, null, &renderer->swapchain);

        // check for errors
        if (result != VK_SUCCESS)
        {
            LogError(CHANNEL, "Swapchain Not Created");
            return false;
        }
        LogSuccess(CHANNEL, "Swapchain Created");
    }

    // create image views
    {
        // get swapchain image count
        vkGetSwapchainImagesKHR(renderer->logicalDevice, renderer->swapchain, &renderer->imageViewCount, null);
        LogInfo(CHANNEL, "Retrieved %d Swapchain Images", renderer->imageViewCount);

        // get swapchain images
        VkImage images[renderer->imageViewCount];
        vkGetSwapchainImagesKHR(renderer->logicalDevice, renderer->swapchain, &renderer->imageViewCount, images);

        // allocate data for image views
        renderer->imageViews = AllocateMemory(renderer->imageViewCount * sizeof(VkImageView));

        // loop thro images
        for (u32 i = 0; i < renderer->imageViewCount; i++)
        {
            // image view creation settings for current image
            VkImageViewCreateInfo imageViewinfo = {};
            imageViewinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewinfo.image = images[i];
            imageViewinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewinfo.format = renderer->imageFormat;
            imageViewinfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewinfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewinfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewinfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewinfo.subresourceRange.layerCount = 1;
            imageViewinfo.subresourceRange.levelCount = 1;
            imageViewinfo.subresourceRange.baseMipLevel = 0;
            imageViewinfo.subresourceRange.baseArrayLayer = 0;

            // create image view
            VkResult result = vkCreateImageView(renderer->logicalDevice, &imageViewinfo, null, &renderer->imageViews[i]);

            // check for errors
            if (result != VK_SUCCESS)
            {
                LogError(CHANNEL, "Image View Creation Failed");
                return false;
            }
            LogSuccess(CHANNEL, "Image View Created");
        }
    }

    // create graphics pipeline
    {
        // store shader modules here
        VkShaderModule vertShader, fragShader;

        // create vertex shader module
        {
            // read vertex shader
            FILE* vertFile = fopen("../bin/default_color.vert.spv", "rb");
            if (!vertFile)
            {
                LogError(CHANNEL, "Vertex Shader Binary Not Found");
                return false;
            }
            fseek(vertFile, 0, SEEK_END);
            u64 vertSize = ftell(vertFile);
            fseek(vertFile, 0, SEEK_SET);
            LogInfo(CHANNEL, "Vertex Shader Binary File Size: %dB", vertSize);
            u32* vertData = RequestStackAllocatorMemory(&allocator, vertSize);
            fread(vertData, 1, vertSize, vertFile);
            fclose(vertFile);

            // vertex shader module info
            VkShaderModuleCreateInfo vertInfo = {};
            vertInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            vertInfo.codeSize = vertSize;
            vertInfo.pCode = vertData;

            // create vertex shader module
            VkResult result = vkCreateShaderModule(renderer->logicalDevice, &vertInfo, null, &vertShader);

            // check for errors
            if (result != VK_SUCCESS)
            {
                LogError(CHANNEL, "Vertex Shader Module Created");
                return false;
            }
            LogSuccess(CHANNEL, "Vertex Shader Module Created");

            // free shader data
            FreeStackAllocatorMemory(&allocator, vertSize);
        }

        // create fragment shader module
        {
            // read fragment shader
            FILE* fragFile = fopen("../bin/default_color.frag.spv", "rb");
            if (!fragFile)
            {
                LogError(CHANNEL, "Fragment Shader Binary Not Found");
                return false;
            }
            fseek(fragFile, 0, SEEK_END);
            u64 fragSize = ftell(fragFile);
            fseek(fragFile, 0, SEEK_SET);
            LogInfo(CHANNEL, "Fragment Shader Binary File Size: %dB", fragSize);
            u32* fragData = RequestStackAllocatorMemory(&allocator, fragSize);
            fread(fragData, 1, fragSize, fragFile);
            fclose(fragFile);

            // vertex shader module info
            VkShaderModuleCreateInfo fragInfo = {};
            fragInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            fragInfo.codeSize = fragSize;
            fragInfo.pCode = fragData;

            // create vertex shader module
            VkResult result = vkCreateShaderModule(renderer->logicalDevice, &fragInfo, null, &fragShader);

            // check for errors
            if (result != VK_SUCCESS)
            {
                LogError(CHANNEL, "Fragment Shader Module Created");
                return false;
            }
            LogSuccess(CHANNEL, "Fragment Shader Module Created");

            // free shader data
            FreeStackAllocatorMemory(&allocator, fragSize);
        }


        // vertex shader stage info
        VkPipelineShaderStageCreateInfo vertStageInfo = {};
        vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStageInfo.module = vertShader;
        vertStageInfo.pName = "main";

        // fragment shader stage info
        VkPipelineShaderStageCreateInfo fragStageInfo = {};
        fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStageInfo.module = fragShader;
        fragStageInfo.pName = "main";

        // store shader stages here
        VkPipelineShaderStageCreateInfo shaderStages[2] = { vertStageInfo, fragStageInfo };

        // define dynamic states
        VkDynamicState dynamicStates[] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        // calculate dyamic state size
        u8 dynamicStateCount = sizeof(dynamicStates) / sizeof(VkDynamicState);
        LogInfo(CHANNEL, "Dynamic State Count: %d", dynamicStateCount);

        // dynamic state info
        VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.dynamicStateCount = dynamicStateCount;
        dynamicStateInfo.pDynamicStates = dynamicStates;

        // specify vertex input
        VkPipelineVertexInputStateCreateInfo vertInputInfo = {};
        vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        // set viewport
        VkViewport viewport = {};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = renderer->extent.width;
        viewport.height = renderer->extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // set scissor
        VkRect2D scissor = {};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = renderer->extent;

        // viewport info
        VkPipelineViewportStateCreateInfo viewportInfo = {};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.scissorCount = 1;

        // rasterization info
        VkPipelineRasterizationStateCreateInfo rasterizationInfo = {};
        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.depthClampEnable = VK_FALSE;
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationInfo.lineWidth = 1.0f;
        rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

        // multisampling info
        VkPipelineMultisampleStateCreateInfo multisampleInfo = {};
        multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleInfo.sampleShadingEnable = VK_FALSE;
        multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // color blending infos
        VkPipelineColorBlendAttachmentState colorBlendAttachInfo = {};
        colorBlendAttachInfo.colorWriteMask = 
            VK_COLOR_COMPONENT_R_BIT | 
            VK_COLOR_COMPONENT_G_BIT | 
            VK_COLOR_COMPONENT_B_BIT | 
            VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachInfo.blendEnable = VK_FALSE;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.logicOpEnable = VK_FALSE;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &colorBlendAttachInfo;

        // pipeline layout info
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        // pipeline layout creation
        {
            VkResult result = vkCreatePipelineLayout(renderer->logicalDevice, &pipelineLayoutInfo, null, &renderer->pipelineLayout);

            // check for errors
            if (result != VK_SUCCESS)
            {
                LogError(CHANNEL, "Pipeline Layout Creation Failed");
                return 0;
            }
            LogSuccess(CHANNEL, "Pipeline Layout Created");
        }

        // attachemnt info
        VkAttachmentDescription attachmentInfo = {};
        attachmentInfo.format = renderer->imageFormat;
        attachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // attachemnt reference
        VkAttachmentReference attachmentRef = {};
        attachmentRef.attachment = 0;
        attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // subpass
        VkSubpassDescription subpassInfo = {};
        subpassInfo.colorAttachmentCount = 1;
        subpassInfo.pColorAttachments = &attachmentRef;

        // renderpass info
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &attachmentInfo;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassInfo;

        // create renderpass
        {
            VkResult result = vkCreateRenderPass(renderer->logicalDevice, &renderPassInfo, null, &renderer->renderPass);
            // check for errors
            if (result != VK_SUCCESS)
            {
                LogError(CHANNEL, "Render Pass Creation Failed");
                return 0;
            }
            LogSuccess(CHANNEL, "Render Pass Created");
        }

        // graphics pipeline info
        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pRasterizationState = &rasterizationInfo;
        pipelineInfo.pMultisampleState = &multisampleInfo;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDynamicState = &dynamicStateInfo;
        pipelineInfo.layout = renderer->pipelineLayout;
        pipelineInfo.renderPass = renderer->renderPass;
        pipelineInfo.subpass = 0;

        // create graphics pipeline
        {
            VkResult result = vkCreateGraphicsPipelines
                (renderer->logicalDevice,
                 VK_NULL_HANDLE,
                 1,
                 &pipelineInfo,
                 null,
                 &renderer->graphicsPipeline);

            // check for errors
            if (result != VK_SUCCESS)
            {
                LogError(CHANNEL, "Graphics Pipeline Creation Failed");
                return 0;
            }
            LogSuccess(CHANNEL, "Graphics Pipeline Created");
        }

        // destoy shader modules
        vkDestroyShaderModule(renderer->logicalDevice, vertShader, null);
        vkDestroyShaderModule(renderer->logicalDevice, fragShader, null);
        LogSuccess(CHANNEL, "Vertex Shader Module Destroyed");
        LogSuccess(CHANNEL, "Fragment Shader Module Destroyed");
    }


    // if code comes here, return success
    LogSuccess(CHANNEL, SYSTEM_INITIALIZED_MESSAGE);
    return true;
}

void ShutdownVKRenderer(void)
{
    // destroy graphics pipeline
    vkDestroyPipeline(renderer->logicalDevice, renderer->graphicsPipeline, null);
    LogSuccess(CHANNEL, "Graphics Pipeline Destroyed");

    // destroy renderpass
    vkDestroyRenderPass(renderer->logicalDevice, renderer->renderPass, null);
    LogSuccess(CHANNEL, "Render Pass Destroyed");

    // destroy pipeline layout
    vkDestroyPipelineLayout(renderer->logicalDevice, renderer->pipelineLayout, null);
    LogSuccess(CHANNEL, "Pipeline Layout Destroyed");

    // loop thro image views
    for (u32 i = 0; i < renderer->imageViewCount; i++)
    {
        // destroy current image view
        vkDestroyImageView(renderer->logicalDevice, renderer->imageViews[i], null);
        LogSuccess(CHANNEL, "Image View Destroyed");
    }

    // free image views array from heap
    FreeMemory(renderer->imageViews, renderer->imageViewCount * sizeof(VkImageView));
    renderer->imageViewCount = 0;

    // destroy swapchain
    vkDestroySwapchainKHR(renderer->logicalDevice, renderer->swapchain, null);
    LogSuccess(CHANNEL, "Swapchain Destroyed");

    // destroy logical device
    vkDestroyDevice(renderer->logicalDevice, null);
    LogSuccess(CHANNEL, "Logical Device Destroyed");

    // destroy messenger if it was created
#if defined(DEBUG)

    DestroyVKMessenger(renderer->instance, renderer->messenger, null);
    LogSuccess(CHANNEL, "Messenger Destroyed");

#endif

    // destroy surface
    vkDestroySurfaceKHR(renderer->instance, renderer->surface, null);
    LogSuccess(CHANNEL, "Surface Destroyed");

    // destroy instance
    vkDestroyInstance(renderer->instance, null);
    LogSuccess(CHANNEL, "Instance Destroyed");

    // destroy stack allocator
    DestroyStackAllocator(&allocator);
    LogSuccess(CHANNEL, SYSTEM_TERMINATED_MESSAGE);
}

void DrawVKRenderer(void)
{
}
