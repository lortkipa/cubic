#include "renderer/renderer.h"
#include "defines.h"
#include "renderer/vulkan_renderer.h"
#include "core/event.h"
#include "core/stack_allocator.h"
#include "core/assert.h"
#include "core/logger.h"

#define CHANNEL "Renderer"

static b8 initialized = false;

static StackAllocator allocator;

static Renderer* renderer;

b8 StartupRenderer(const RendererBackend backend)
{
    // make sure system is not started
    Assert(CHANNEL, initialized == false, "System Is Already Initialized");

    // create stack allocator
    if (!CreateStackAllocator(&allocator, 2 * sizeof(Renderer)))
    {
        LogError(CHANNEL, "Allocator Creation Failed");
        return false;
    }

    // request memory for renderer structure
    renderer = RequestStackAllocatorMemory(&allocator, sizeof(Renderer));
    
    // check backend and asign function pointers
    switch (backend) 
    {
        case RENDERER_BACKEND_VULKAN:
            renderer->Startup = StartupVKRenderer;
            renderer->Shutdown = ShutdownVKRenderer;
            renderer->DrawFrame = DrawVKFrame;
            LogInfo(CHANNEL, "Backend Choosen: \"%s\"", "Vulkan");
            break;
        default:
            LogError(CHANNEL, "Invalid Backend Provided");
            return false;
            break;
    }

    // startup provided backend
    if (!renderer->Startup())
    {
        LogError(CHANNEL, "Backend Not Initialized");
        return false;
    }

    // init default clear color
    renderer->background = (Color) { 50, 50, 50, 255 };

    // if code comes here, everything went great, so return success
    initialized = true;
    LogSuccess(CHANNEL, SYSTEM_INITIALIZED_MESSAGE);
    return true;
}

void ShutdownRenderer(void)
{
    // make sure system is started
    Assert(CHANNEL, initialized, "System Not Initialized Yet");

    // shutdown backend
    renderer->Shutdown();

    // destroy allocator
    DestroyStackAllocator(&allocator);

    // shutdown system
    initialized = false;
    LogSuccess(CHANNEL, SYSTEM_TERMINATED_MESSAGE);
}

void DrawFrame(void)
{
    // make sure system is started
    Assert(CHANNEL, initialized, "System Not Initialized Yet");

    // draw frame with backend
    renderer->DrawFrame();
}
