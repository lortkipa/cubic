#include <core/logger.h>

int main(void)
{
    // startup systems
    StartupLogSystem(LOG_VERBOSITY_FLAG_ERROR | LOG_VERBOSITY_FLAG_WARNING | LOG_VERBOSITY_FLAG_SUCCESS | LOG_VERBOSITY_FLAG_INFO);

    LogError("Game", "Hello World %f", 3.14f);
    LogWarning("Game", "Hello World %f", 3.14f);
    LogSuccess("Game", "Hello World %f", 3.14f);
    LogInfo("Game", "Hello World %f", 3.14f);

    // shutdown systems
    ShutdownLogSystem();
}
