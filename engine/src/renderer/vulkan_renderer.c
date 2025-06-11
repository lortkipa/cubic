#include "renderer/vulkan_renderer.h"
#include "platform/vulkan_extensions.h"
#include "core/stack_allocator.h"
#include "core/logger.h"

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
    if (!CreateStackAllocator(&allocator, sizeof(VKRenderer) + 1000))
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

            // loop thro queue families
            for (u32 j = 0; j < queueFamilyCount; j++)
            {
                // try to find graphics family index
                if (!graphicsQueueFamilyFound && (queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT))
                {
                    graphicsQueueFamilyFound = true;
                    LogInfo(CHANNEL, "Graphics Queue Family Found At Index: %d", j);
                }

                // save family indinces and exit the loop, if all indinces are found
                if (graphicsQueueFamilyFound)
                {
                    renderer->queueFamilyIndinces.graphicsFamily = i;
                    break;
                }
            }

            // free queue families from heap
            RequestStackAllocatorMemory(&allocator, queueFamilyCount * sizeof(VkQueueFamilyProperties));

            // try to pick gpu
            if (graphicsQueueFamilyFound && features.geometryShader)
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
        // logical device queue info
        VkDeviceQueueCreateInfo logicalDeviceQueueInfo = {};
        logicalDeviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        logicalDeviceQueueInfo.queueFamilyIndex = renderer->queueFamilyIndinces.graphicsFamily;
        logicalDeviceQueueInfo.queueCount = 1;
        float queuePriority = 1.0f;
        logicalDeviceQueueInfo.pQueuePriorities = &queuePriority;

        // set app features
        VkPhysicalDeviceFeatures rendererFeatures = {};

        // logical device info
        VkDeviceCreateInfo logicalDeviceInfo = {};
        logicalDeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        logicalDeviceInfo.pQueueCreateInfos = &logicalDeviceQueueInfo;
        logicalDeviceInfo.queueCreateInfoCount = 1;
        logicalDeviceInfo.pEnabledFeatures = &rendererFeatures;

        // create logical device
        VkResult result = vkCreateDevice(renderer->physicalDevice, &logicalDeviceInfo, null, &renderer->logicalDevice);

        // check for errors
        if (result != VK_SUCCESS)
        {
            LogError(CHANNEL, "Logical Device Not Created");
            return 0;
        }
        LogSuccess(CHANNEL, "Logical Device Created");
    }

    // if code comes here, return success
    LogSuccess(CHANNEL, SYSTEM_INITIALIZED_MESSAGE);
    return true;
}

void ShutdownVKRenderer(void)
{
    // destroy logical device
    vkDestroyDevice(renderer->logicalDevice, null);
    LogSuccess(CHANNEL, "Logical Device Destroyed");

    // destroy messenger if it was created
#if defined(DEBUG)

    DestroyVKMessenger(renderer->instance, renderer->messenger, null);
    LogSuccess(CHANNEL, "Messenger Destroyed");

#endif

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
