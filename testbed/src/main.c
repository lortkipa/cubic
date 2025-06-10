#include <core/logger.h>
#include <core/memory.h>
#include <core/event.h>
#include <platform/window.h>
#include <renderer/renderer.h>

static b8 isRunning = true;

void onCloseRequest(const Event* p_event)
{
    isRunning = false;
}

void onResize(const Event* p_event)
{
    LogInfo("Game", "Window Resized %dx%d",
            GetEventArg(p_event, "Width").asI32,
            GetEventArg(p_event, "Height").asI32
            );
}

void onPress(const Event* p_event)
{
    LogInfo("Game", "Key Pressed %d", GetEventArg(p_event, "Key").asI32);
}

int main(void)
{
    // startup systems
    StartupLogSystem(LOG_VERBOSITY_FLAG_ERROR | LOG_VERBOSITY_FLAG_WARNING | LOG_VERBOSITY_FLAG_SUCCESS | LOG_VERBOSITY_FLAG_INFO);
    StartupMemorySystem();
    StartupEventSystem();
    SubToEvent(EVENT_TYPE_WINDOW_EXIT_REQUEST, onCloseRequest);
    SubToEvent(EVENT_TYPE_WINDOW_RESIZE, onResize);
    SubToEvent(EVENT_TYPE_KEY_PRESS, onPress);
    CreateWindow(1000, 800, "Cubic Game");
    StartupEngine();

    // game loop
    while (isRunning)
    {
        // poll & process events
        FireWindowEvents();
        ProcessEvents();
    }

    // shutdown systems
    ShutdownEngine();
    DestroyWindow();
    UnsubToEvent(EVENT_TYPE_WINDOW_EXIT_REQUEST, onCloseRequest);
    UnsubToEvent(EVENT_TYPE_WINDOW_RESIZE, onResize);
    UnsubToEvent(EVENT_TYPE_KEY_PRESS, onPress);
    ShutdownEventSystem();
    ShutdownMemorySystem();
    ShutdownLogSystem();
}
