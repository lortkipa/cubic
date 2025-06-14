#include "renderer/renderer.h"
#include "platform/context.h"
#include "core/event.h"
#include "core/stack_allocator.h"
#include "core/assert.h"
#include "core/logger.h"
#include "defines.h"
#include <GL/glew.h>

#define CHANNEL "Renderer"

static b8 initialized = false;

static StackAllocator allocator;

static Renderer* renderer;

static void WindowResizeCallback(const Event* p_event);

b8 StartupRenderer(void)
{
    // make sure system is not started
    Assert(CHANNEL, initialized == false, "System Is Already Initialized");

    // create opengl context
    CreateGLContext(3, 3);
    BindGLContext();

    // init glew library
    if (glewInit() != GLEW_OK)
    {
        LogError(CHANNEL, "Glew Library Not Initialized");
        return false;
    }
    LogSuccess(CHANNEL, "Glew Library Initialized");

    // set window resize callback
    SubToEvent(EVENT_TYPE_WINDOW_RESIZE, WindowResizeCallback);

    // create stack allocator
    if (!CreateStackAllocator(&allocator, 2 * sizeof(Renderer)))
    {
        LogError(CHANNEL, "Allocator Creation Failed");
        return false;
    }

    // request memory for renderer structure
    renderer = RequestStackAllocatorMemory(&allocator, sizeof(Renderer));

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

    // shutdown system
    initialized = false;
    LogSuccess(CHANNEL, SYSTEM_TERMINATED_MESSAGE);
}

void DrawFrame(void)
{
    // make sure system is started
    Assert(CHANNEL, initialized, "System Not Initialized Yet");

    // clear color and set background
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(renderer->background.r / 255.0f, renderer->background.g / 255.0f, renderer->background.b / 255.0f, renderer->background.a / 255.0f);

    // swap buffers
    SwapGLBuffers();
}

static void WindowResizeCallback(const Event* p_event)
{
    // set viewport for whole window
    glViewport(0, 0, GetEventArg(p_event, "Width").asU32, GetEventArg(p_event, "Height").asU32);
}
