#include <core/logger.h>
#include <core/memory.h>

int main(void)
{
    // startup systems
    StartupLogSystem(LOG_VERBOSITY_FLAG_ERROR | LOG_VERBOSITY_FLAG_WARNING | LOG_VERBOSITY_FLAG_SUCCESS | LOG_VERBOSITY_FLAG_INFO);
    StartupMemorySystem();

    // shutdown systems
    ShutdownMemorySystem();
    ShutdownLogSystem();
}
