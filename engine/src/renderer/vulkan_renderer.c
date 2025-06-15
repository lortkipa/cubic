#include "renderer/vulkan_renderer.h"
#include "platform/vulkan_platform.h"
#include "core/stack_allocator.h"
#include "core/logger.h"
#include <errno.h>

#define CHANNEL "Vulkan Renderer"

static StackAllocator allocator;

static VKRenderer* renderer;

static b8 CreateVKInstance(void);
static void DestroyVKInstance(void);
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

b8 StartupVKRenderer(void) {
    // create allocator
    if (!CreateStackAllocator(&allocator, 2 * sizeof(VKRenderer))) {
        LogError(CHANNEL, "Stack Allocator Creation Failed");
        return false;
    }

    // request memory for renderer
    renderer = RequestStackAllocatorMemory(&allocator, sizeof(VKRenderer));

    // init renderer stuff
    if (!CreateVKInstance())
        return false;
    if (!CreateVKDebugMessenger())
        return false;
    if (!ChooseVKPhysicalDevice())
        return false;
    if (!CreateVKDevice())
        return false;

    // if code comes here, everything is good, so return success
    LogSuccess(CHANNEL, SYSTEM_INITIALIZED_MESSAGE);
    return true;
}

void ShutdownVKRenderer(void)
{
    // terminate renderer stuff
    DestroyVKDevice();
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

        // loop queue families
        for (u32 j = 0; j < queueFamilyCount; j++)
        {
            // check if graphics queue is found
            if (queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsFamilyFound = true;
                LogInfo(CHANNEL, "Founded Graphics Queue Family For Device \"%s\" At Index: %d",
                        properties.deviceName, j);
            }

            // if all queue family is found, store indinces and break the loop
            if (graphicsFamilyFound)
            {
                renderer->graphicsQueueIndex = j;
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

    // queue create info
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueCount = 1,
        .queueFamilyIndex = renderer->graphicsQueueIndex,
        .pQueuePriorities = &queuePriority
    };

    // device extensions
    const char* exts[] = 
    {
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
    
    // device info
    VkDeviceCreateInfo deviceInfo =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pEnabledFeatures = &features,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueInfo,
        .enabledExtensionCount = extCount,
        .ppEnabledExtensionNames = exts
    };

    // create device
    VkResult result = vkCreateDevice(renderer->GPU, &deviceInfo, null, &renderer->device);

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
    vkDestroyDevice(renderer->device, null);
    LogSuccess(CHANNEL, "Device Destroyed");
}
