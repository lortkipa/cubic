#include <complex.h>
#include <platform/window.h>
#include <core/logger.h>
#include <core/memory.h>
#include <core/event.h>

static bool isRunning = true;

void onWindowExit(Event* p_event)
{
    isRunning = false;
}

int main(void)
{
    StartupLogSystem(LOG_VERBOSITY_FLAG_EVERY);

    StartupMemorySystem();

    StartupEventSystem();
    SubscribeToEvent(EVENT_TYPE_WINDOW_EXIT_REQUEST, onWindowExit);

    CreateWindow(1000, 800, "Cubic Engine");

    while (isRunning)
    {
        FireWindowEvents();
        ProcessEvents();
    }

    destroyWindow();

    UnsubscribeToEvent(EVENT_TYPE_WINDOW_EXIT_REQUEST, onWindowExit);
    ShutdownEventSystem();

    ShutdownMemorySystem();

    ShutdownLogSystem();
}
