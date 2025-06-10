#include <core/logger.h>
#include <core/memory.h>
#include <core/event.h>
#include <platform/window.h>

static b8 isRunning = true;

void onCloseRequest(const Event* p_event)
{
    isRunning = false;
}

void onResize(const Event* p_event)
{
    LogInfo("Game", "Window Resized %dx%d",
            GetEventArgI32(p_event, "Width"),
            GetEventArgI32(p_event, "Height")
            );
}

void onPress(const Event* p_event)
{
    LogInfo("Game", "Key Pressed %d", GetEventArgI32(p_event, "Key"));
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

    while (isRunning)
    {
        FireWindowEvents();
        ProcessEvents();
    }

    // shutdown systems
    DestroyWindow();
    ShutdownEventSystem();
    ShutdownMemorySystem();
    ShutdownLogSystem();
}
