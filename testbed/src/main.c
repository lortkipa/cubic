#include <core/logger.h>
#include <core/memory.h>
#include <core/event.h>
#include <platform/window.h>

static b8 isRunning = true;

void onCloseRequest(const Event* p_event)
{
    isRunning = false;
}

int main(void)
{
    // startup systems
    StartupLogSystem(LOG_VERBOSITY_FLAG_ERROR | LOG_VERBOSITY_FLAG_WARNING | LOG_VERBOSITY_FLAG_SUCCESS | LOG_VERBOSITY_FLAG_INFO);
    StartupMemorySystem();
    StartupEventSystem();
    SubToEvent(EVENT_TYPE_WINDOW_EXIT_REQUEST, onCloseRequest);
    CreateWindow(1000, 800, "Cubic Game");

    // game loop
    while (isRunning)
    {
        // poll & process events
        FireWindowEvents();
        ProcessEvents();
    }

    // shutdown systems
    DestroyWindow();
    UnsubToEvent(EVENT_TYPE_WINDOW_EXIT_REQUEST, onCloseRequest);
    ShutdownEventSystem();
    ShutdownMemorySystem();
    ShutdownLogSystem();
}
