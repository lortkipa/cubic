#include "renderer/vulkan_renderer.h"
#include "platform/vulkan_extensions.h"
#include "core/stack_allocator.h"
#include "core/logger.h"

#define CHANNEL "Vulkan Renderer"

static StackAllocator allocator;

static VKRenderer* renderer;

b8 StartupVKRenderer(void)
{
    // create allocator
    if (!CreateStackAllocator(&allocator, 2 * sizeof(VKRenderer)))
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
        const char** exts = GetRequiredVKInstanceExtensions(&extCount);

        // log extensions in debug mode
#if defined(DEBUG)

        for (u8 i = 0; i < extCount; i++)
        {
            LogInfo(CHANNEL, "Loading Extension: %s", exts[i]);
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

    // if code comes here, return success
    return true;
}

void ShutdownVKRenderer(void)
{
    // destroy instance
    vkDestroyInstance(renderer->instance, null);
    LogSuccess(CHANNEL, "Instance Destroyed");

    // destroy stack allocator
    DestroyStackAllocator(&allocator);
}

void DrawVKRenderer(void)
{
}
