#include "renderer/vulkan_renderer.h"
#include "core/stack_allocator.h"
#include "core/logger.h"

#define CHANNEL "Vulkan Renderer"

static StackAllocator allocator;

static VKRenderer* renderer;

static b8 CreateVKInstance(void);
static void DestroyVKInstance();

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

    // if code comes here, everything is good, so return success
    LogSuccess(CHANNEL, SYSTEM_INITIALIZED_MESSAGE);
    return true;
}

void ShutdownVKRenderer(void)
{
    // terminate renderer stuff
    DestroyVKInstance();

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

    // instance info
    VkInstanceCreateInfo instanceInfo = 
    {
        .sType = instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo
    };

    // create instance
    VkResult result = vkCreateInstance(&instanceInfo, null, &renderer->instance);

    // check for errors
    if (result != VK_SUCCESS)
    {
        LogError(CHANNEL, "Instance Creation Failed");
        return false;
    }

    LogSuccess(CHANNEL, "Instance Created");
    return true;
}

static void DestroyVKInstance()
{
    // destroy instance
    vkDestroyInstance(renderer->instance, null);
    LogSuccess(CHANNEL, "Instance Destroyed");
}
