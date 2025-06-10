#include <core/logger.h>
#include <core/memory.h>
#include <platform/window.h>

int main(void)
{
    // startup systems
    StartupLogSystem(LOG_VERBOSITY_FLAG_ERROR | LOG_VERBOSITY_FLAG_WARNING | LOG_VERBOSITY_FLAG_SUCCESS | LOG_VERBOSITY_FLAG_INFO);
    StartupMemorySystem();
    CreateWindow(1000, 800, "Cubic Game");

    while (true)
    {
        FireWindowEvents();
    }

    // shutdown systems
    DestroyWindow();
    ShutdownMemorySystem();
    ShutdownLogSystem();
}
