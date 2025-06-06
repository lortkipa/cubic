#include <platform/window.h>
#include <core/logger.h>
#include <core/assert.h>
#include <core/memory.h>

int main(void)
{
    StartupLogSystem
        (
         LOG_VERBOSITY_FLAG_ERROR | LOG_VERBOSITY_FLAG_WARNING |
         LOG_VERBOSITY_FLAG_SUCCESS | LOG_VERBOSITY_FLAG_INFO
        );
    StartupMemorySystem();

    CreateWindow(1000, 800, "Cubic Engine");
    while (true)
    {
        FireWindowEvents();
    }
    destroyWindow();

    ShutdownMemorySystem();
    ShutdownLogSystem();
}
