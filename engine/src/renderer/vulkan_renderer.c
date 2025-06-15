#include "renderer/vulkan_renderer.h"
#include "platform/vulkan_platform.h"
#include "platform/window.h"
#include "core/stack_allocator.h"
#include "core/logger.h"

#define CHANNEL "Vulkan Renderer"

static StackAllocator allocator;

static VKRenderer* renderer;

static b8 CreateVKInstance(void);
static void DestroyVKInstance(void);
static void DestroySurface(void);
static b8 CreateVKDebugMessenger(void);
static void DestroyVKDebugMessenger(void);
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugVKCallback
(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
 VkDebugUtilsMessageTypeFlagBitsEXT type,
 const VkDebugUtilsMessengerCallbackDataEXT* p_callbackData,
 void* userData);
static b8 ChooseVKPhysicalDevice(void);
static b8 CreateVKDevice(void);
static void DestroyVKDevice(void);
static void GetVKQueueHandles(void);
static b8 CreateVKSwapchain(void);
static void DestroyVKSwapchain(void);

b8 StartupVKRenderer(void) 
{
    // create allocator
    if (!CreateStackAllocator(&allocator, 2 * sizeof(VKRenderer))) 
    {
        LogError(CHANNEL, "Stack Allocator Creation Failed");
        return false;
    }

    // request memory for renderer
    renderer = RequestStackAllocatorMemory(&allocator, sizeof(VKRenderer));

    // init renderer stuff
    if (!CreateVKInstance())
        return false;
    if (!CreateVKSurface(renderer->Instance, &renderer->Surface))
        return false;
    if (!CreateVKDebugMessenger())
        return false;
    if (!ChooseVKPhysicalDevice())
        return false;
    if (!CreateVKDevice())
        return false;
    GetVKQueueHandles();
    if (!CreateVKSwapchain())
        return false;

    // if code comes here, everything is good, so return success
    LogSuccess(CHANNEL, SYSTEM_INITIALIZED_MESSAGE);
    return true;
}

void ShutdownVKRenderer(void)
{
    // terminate renderer stuff
    DestroyVKSwapchain();
    DestroyVKDevice();
    DestroySurface();
    DestroyVKDebugMessenger();
    DestroyVKInstance();

    // free renderer from heap
    FreeStackAllocatorMemory(&allocator, sizeof(VKRenderer));

    // destoy allocator
    DestroyStackAllocator(&allocator);
    LogSuccess(CHANNEL, SYSTEM_TERMINATED_MESSAGE);
}

void DrawVKFrame(void)
{
}

static b8 CreateVKInstance(void)
{
    // application info
    VkApplicationInfo appInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Cubic Engine",
        .apiVersion = VK_API_VERSION_1_3
    };

    // get needed extensions
    u8 extCount;
    const char** exts = GetVKInstanceExtensions(&extCount);

    // log extensions in debug mode
#if defined(DEBUG)
    for (u8 i = 0; i < extCount; i++)
    {
        LogInfo(CHANNEL, "Loading Instance Extensions: \"%s\"", exts[i]);
    }
#endif

    // define validation layers depending on debug mode
#if defined(DEBUG)
    const char* layers[]  = { "VK_LAYER_KHRONOS_validation" };
#else
    const char* layers[0];
#endif

    // calculate layer count dynamicly
    u8 layerCount = sizeof(layers) / sizeof(char*);

    // log layers in debug mode
#if defined(DEBUG)
    for (u8 i = 0; i < layerCount; i++)
    {
        LogInfo(CHANNEL, "Loading Instance Layer: \"%s\"", layers[i]);
    }
#endif

    // instance info
    VkInstanceCreateInfo instanceInfo = 
    {
        .sType = instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = extCount,
        .ppEnabledExtensionNames = exts,
        .enabledLayerCount = layerCount, 
        .ppEnabledLayerNames = layers
    };

    // create instance
    VkResult result = vkCreateInstance(&instanceInfo, null, &renderer->Instance);

    // check for errors
    if (result != VK_SUCCESS)
    {
        LogError(CHANNEL, "Instance Creation Failed");
        return false;
    }

    LogSuccess(CHANNEL, "Instance Created");
    return true;
}

static void DestroyVKInstance(void)
{
    // destroy instance
    vkDestroyInstance(renderer->Instance, null);
    LogSuccess(CHANNEL, "Instance Destroyed");
}

static void DestroySurface(void)
{
    // destroy surface
    vkDestroySurfaceKHR(renderer->Instance, renderer->Surface, null);
    LogSuccess(CHANNEL, "Surface Destroyed");
}

static b8 CreateVKDebugMessenger(void)
{
    // create debug messenger on debug mode only
#if defined(DEBUG)

    // vulkan messenger info
    VkDebugUtilsMessengerCreateInfoEXT messengerInfo = 
    {
        .sType = messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
        .messageType = 
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = DebugVKCallback
    };

    // load function
    auto CreateMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(renderer->Instance, "vkCreateDebugUtilsMessengerEXT");

    // check if function is loaded
    if (!CreateMessenger)
    {
        LogError(CHANNEL, "vkCreateDebugUtilsMessengeinstancerEXT() Not Loaded");
        return false;
    }

    // create vulkan messenger
    VkResult result = CreateMessenger(renderer->Instance, &messengerInfo, null, &renderer->DebugMessenger);

    // check result
    if (result != VK_SUCCESS)
    {
        LogError(CHANNEL, "Messenger Creation Failed");
        return false;
    }
    LogSuccess(CHANNEL, "Messenger Created");

#endif

    return true;
}

static void DestroyVKDebugMessenger(void)
{
    // destroy (and create) debug messenger on debug mode only
#if defined(DEBUG)

    // load function
    auto DestroyMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(renderer->Instance, "vkDestroyDebugUtilsMessengerEXT");

    // check if function is loaded
    if (!DestroyMessenger)
    {
        LogError(CHANNEL, "vkDestroyDebugUtilsMessengeinstancerEXT() Not Loaded");
    }
    else 
    {
        // destroy messenger
        DestroyMessenger(renderer->Instance, renderer->DebugMessenger, null);
        LogSuccess(CHANNEL, "Messenger Destroyed");
    }

#endif
}

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugVKCallback
(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
 VkDebugUtilsMessageTypeFlagBitsEXT type,
 const VkDebugUtilsMessengerCallbackDataEXT* p_callbackData,
 void* userData)
{
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        LogError("Vulkan Messenger", "%s",
                p_callbackData->pMessage);
    }
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        LogWarning("Vulkan Messenger", "%s",
                p_callbackData->pMessage);
    }
    else 
    {
        LogInfo("Vulkan Messenger", "%s",
                p_callbackData->pMessage);
    }

    // default return value
    return VK_FALSE;
}

static b8 ChooseVKPhysicalDevice(void)
{
    // get gpu count
    u32 gpuCount;
    vkEnumeratePhysicalDevices(renderer->Instance, &gpuCount, null);
    LogInfo(CHANNEL, "GPU Found: %d", gpuCount);

    // get gpus
    VkPhysicalDevice* gpus = RequestStackAllocatorMemory(&allocator, gpuCount * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(renderer->Instance, &gpuCount, gpus);

    // check if at list 1 gpu is found
    if (gpuCount == 0)
    {
        LogError(CHANNEL, "No GPU Found");
        return false;
    }

    // log gpus in debug mode
#if defined(DEBUG)
    for (u32 i = 0; i < gpuCount; i++)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(gpus[i], &properties);
        LogInfo(CHANNEL, "GPU Found: \"%s\". Max Supported VK Version: %d.%d",
                properties.deviceName, VK_API_VERSION_MAJOR(properties.apiVersion), VK_API_VERSION_MINOR(properties.apiVersion));
    }
#endif

    // track if gpu is found
    b8 gpuFound = false;

    // loop gpus
    for (u32 i = 0; i < gpuCount; i++)
    {
        // get physical device features and properties
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceProperties(gpus[i], &properties);
        vkGetPhysicalDeviceFeatures(gpus[i], &features);

        // get queue family count
        u32 queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(gpus[i], &queueFamilyCount, null);
        LogInfo(CHANNEL, "Queue Family Found For Device \"%s\": %d",
                properties.deviceName, queueFamilyCount);

        // get queue families
        VkQueueFamilyProperties queueFamilies[queueFamilyCount];
        vkGetPhysicalDeviceQueueFamilyProperties(gpus[i], &queueFamilyCount, queueFamilies);

        // track wich queue families where found
        b8 graphicsFamilyFound = false;
        b8 presentFamilyFound = false;

        // loop queue families
        for (u32 j = 0; j < queueFamilyCount; j++)
        {
            // check if graphics queue is found
            if (!graphicsFamilyFound && queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                renderer->GraphicsQueueFamilyIndex = j;
                graphicsFamilyFound = true;
                LogInfo(CHANNEL, "Founded Graphics Queue Family For Device \"%s\" At Index: %d",
                        properties.deviceName, j);
            }

            // check if present queue is found
            VkBool32 _presentFamilyFound;
            vkGetPhysicalDeviceSurfaceSupportKHR(gpus[i], j, renderer->Surface, &_presentFamilyFound);
            if (!presentFamilyFound && _presentFamilyFound)
            {
                renderer->PresentQueueFamilyIndex = j;
                presentFamilyFound = true;
                LogInfo(CHANNEL, "Founded Present Queue Family For Device \"%s\" At Index: %d",
                        properties.deviceName, j);
            }

            // if all queue family is found and break the loop
            if (graphicsFamilyFound && presentFamilyFound)
            {
                break;
            }
        }

        // if suitable physical device is found, stop the loop and store device
        if (features.geometryShader)
        {
            renderer->GPU = gpus[i];
            gpuFound = true;
            LogSuccess(CHANNEL, "Suitable GPU Found: \"%s\"", properties.deviceName);
            break;
        }
        else 
        {
            LogInfo(CHANNEL, "GPU \"%s\" Not Choosen", properties.deviceName);
        }
    }

    // free gpus from heap
    FreeStackAllocatorMemory(&allocator, gpuCount * sizeof(VkPhysicalDevice));

    // if gpu is not found, return failure
    if (!gpuFound)
    {
        LogError(CHANNEL, "Suitable GPU Not Found");
        return false;
    }

    // if code comes here, gpu is found, so return success
    return true;
}

static b8 CreateVKDevice(void)
{
    // enabled features
    VkPhysicalDeviceFeatures features =
    {
        .geometryShader = true 
    };

    // create queue create info
    u8 queueInfoCount = renderer->GraphicsQueueFamilyIndex == renderer->PresentQueueFamilyIndex ? 1 : 2;
    VkDeviceQueueCreateInfo* queueInfos = RequestStackAllocatorMemory(&allocator, queueInfoCount * sizeof(VkDeviceQueueCreateInfo));
    float queuePriority = 1.0f;
    for (u8 i = 0; i < queueInfoCount; i++)
    {
        VkDeviceQueueCreateInfo queueInfo = 
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueCount = 1,
            .queueFamilyIndex = i == 0 ? renderer->GraphicsQueueFamilyIndex : renderer->PresentQueueFamilyIndex,
            .pQueuePriorities = &queuePriority
        };
        queueInfos[i] = queueInfo;
    }
    LogInfo(CHANNEL, "Queue Info Structs Created: %d", queueInfoCount);

    // device extensions
    const char* exts[] = 
    {
        "VK_KHR_swapchain",
        "VK_KHR_dynamic_rendering"
    };

    // calculate ext count dynamicly
    u32 extCount = sizeof(exts) / sizeof(char*);

    // log instance extensions in debug mode
#if defined(DEBUG)
    for (u32 i = 0; i < extCount; i++)
    {
        LogInfo(CHANNEL, "Loading Instance Extensions: \"%s\"", exts[i]);
    }
#endif

    // dynamic rendering info
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingInfo = {};
    dynamicRenderingInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamicRenderingInfo.dynamicRendering = VK_TRUE;

    // device info
    VkDeviceCreateInfo deviceInfo =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &dynamicRenderingInfo,
        .pEnabledFeatures = &features,
        .queueCreateInfoCount = queueInfoCount,
        .pQueueCreateInfos = queueInfos,
        .enabledExtensionCount = extCount,
        .ppEnabledExtensionNames = exts
    };

    // create device
    VkResult result = vkCreateDevice(renderer->GPU, &deviceInfo, null, &renderer->Device);

    // free queue infos from heap
    FreeStackAllocatorMemory(&allocator, queueInfoCount * sizeof(VkDeviceQueueCreateInfo));

    // check if device was created
    if (result != VK_SUCCESS)
    {
        LogError(CHANNEL, "Device Creation Failed");
        return false;
    }

    LogSuccess(CHANNEL, "Device Created");
    return true;
}

static void DestroyVKDevice(void)
{
    // destroy device
    vkDestroyDevice(renderer->Device, null);
    LogSuccess(CHANNEL, "Device Destroyed");
}

static void GetVKQueueHandles(void)
{
    // get graphics queue handle
    vkGetDeviceQueue(renderer->Device,
            renderer->GraphicsQueueFamilyIndex,
            0,
            &renderer->GraphicsQueue);

    // get present queue handle
    vkGetDeviceQueue(renderer->Device,
            renderer->PresentQueueFamilyIndex,
            0,
            &renderer->PresentQueue);

    LogInfo(CHANNEL, "Got Graphics And Present Queue Handles");
}

static b8 CreateVKSwapchain(void)
{
    // get present mode count
    u32 presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->GPU, renderer->Surface, &presentModeCount, null);
    LogInfo(CHANNEL, "Present Mode Found: %d", presentModeCount);

    // get present modes
    VkPresentModeKHR presentModes[presentModeCount];
    vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->GPU, renderer->Surface, &presentModeCount, presentModes);

    // store present mode here
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    // choose best present mode
    for (u32 i = 0; i < presentModeCount; i++)
    {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }
    LogInfo(CHANNEL, "Suitable Present Mode Found: %s",
            presentMode == VK_PRESENT_MODE_MAILBOX_KHR ? "Mailbox" : "Fifo");

    // get surface capabilities
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer->GPU, renderer->Surface, &surfaceCapabilities);
    LogInfo(CHANNEL, "Surface Min Image Count: %d", surfaceCapabilities.minImageCount);
    LogInfo(CHANNEL, "SSurface Max Image Count: %d", surfaceCapabilities.maxImageCount);

    // create extent
    VkExtent2D extent;
    if (surfaceCapabilities.currentExtent.width != UINT32_MAX)
    {
        extent = surfaceCapabilities.currentExtent;
    }
    else 
    {
        WindowSize size = GetWindowSize();

        if (size.width > surfaceCapabilities.maxImageExtent.width)
        {
            extent.width = surfaceCapabilities.maxImageExtent.width;
        }
        else if (size.width < surfaceCapabilities.minImageExtent.width)
        {
            extent.width = surfaceCapabilities.minImageExtent.width;
        }
        else 
        {
            extent.width = size.width;
        }

        if (size.height > surfaceCapabilities.maxImageExtent.height)
        {
            extent.height = surfaceCapabilities.maxImageExtent.height;
        }
        else if (size.height < surfaceCapabilities.minImageExtent.height)
        {
            extent.height = surfaceCapabilities.minImageExtent.height;
        }
        else 
        {
            extent.height = size.height;
        }
    }
    LogInfo(CHANNEL, "Swapchain Extent Width Choosen: %d", extent.width);
    LogInfo(CHANNEL, "Swapchain Extent Height Choosen: %d", extent.height);

    // get surface format count
    u32 surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->GPU, renderer->Surface, &surfaceFormatCount, null);
    LogInfo(CHANNEL, "Surface Format Found: %d", surfaceFormatCount);

    // get surface formats
    VkSurfaceFormatKHR* surfaceFormats = RequestStackAllocatorMemory(&allocator, surfaceFormatCount * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->GPU, renderer->Surface, &surfaceFormatCount, surfaceFormats);

    // choose best surface format
    VkSurfaceFormatKHR surfaceFormat = surfaceFormats[0];
    for (u32 i = 0; i < surfaceFormatCount; i++)
    {
        if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB 
                && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surfaceFormat = surfaceFormats[i];
            break;
        }
    }

    // free surface formats
    FreeStackAllocatorMemory(&allocator, surfaceFormatCount * sizeof(VkSurfaceFormatKHR));

    // calculate correct image count
    u32 imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }
    LogInfo(CHANNEL, "Swapchain Image Count Choose: %d", imageCount);

    // swapchain info
    VkSwapchainCreateInfoKHR swapchainInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = renderer->Surface,
        .presentMode = presentMode,
        .imageExtent = extent,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = 
            (renderer->GraphicsQueueFamilyIndex == renderer->PresentQueueFamilyIndex) ?
            VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount =
            (renderer->GraphicsQueueFamilyIndex == renderer->PresentQueueFamilyIndex) ?
            0 : 2,
        .pQueueFamilyIndices = 
            (renderer->GraphicsQueueFamilyIndex == renderer->PresentQueueFamilyIndex) ?
            null : (u32[]){renderer->GraphicsQueueFamilyIndex, renderer->PresentQueueFamilyIndex},
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    // create swapchain
    VkResult result = vkCreateSwapchainKHR(renderer->Device, &swapchainInfo, null, &renderer->Swapchain);

    // check if swapchain is created
    if (result != VK_SUCCESS)
    {
        LogError(CHANNEL, "Swapchain Not Created");
        return false;
    }

    LogSuccess(CHANNEL, "Swapchain Created");
    return true;
}

static void DestroyVKSwapchain(void)
{
    // destroy swapchain
    vkDestroySwapchainKHR(renderer->Device, renderer->Swapchain, null);
    LogSuccess(CHANNEL, "Swapchain Destroyed");
}
