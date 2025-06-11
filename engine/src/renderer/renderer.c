#include "renderer/renderer.h"
#include "renderer/vulkan_renderer.h"
#include "core/assert.h"
#include "core/logger.h"

#define CHANNEL "Renderer"

static b8 initialized = false;
static Renderer renderer;

b8 StartupRenderer(const RendererBackend backend)
{
    // make sure system is not started
    Assert(CHANNEL, !initialized, "System Is Already Initialized");

    // load functions
    switch (backend)
    {
        case RENDERER_BACKEND_VULKAN:
            renderer.Startup = StartupVKRenderer;
            renderer.Shutdown = ShutdownVKRenderer;
            renderer.Draw = DrawVKRenderer;
            break;
        default:
            LogError(CHANNEL, "Invalid Backend Provided: %d", backend);
            return false;
            break;
    }

    // startup renderer
    if (!renderer.Startup())
    {
        LogError(CHANNEL, SYSTEM_NOT_INITIALIZED_MESSAGE); 
        return false;
    }

    // track startup
    initialized = true;
    LogSuccess(CHANNEL, SYSTEM_INITIALIZED_MESSAGE);

    // return success
    return true;
}

void ShutdownRenderer(void)
{
    // make sure system is started
    Assert(CHANNEL, initialized, "System Is Not Initialized Yet");

    // shutdown renderer
    renderer.Shutdown();

    // track shutdown
    initialized = false;
    LogSuccess(CHANNEL, SYSTEM_TERMINATED_MESSAGE);
}

void RunRenderer(void)
{
    // make sure system is started
    Assert(CHANNEL, initialized, "System Is Not Initialized Yet");

    // draw with renderer
    renderer.Draw();
}
