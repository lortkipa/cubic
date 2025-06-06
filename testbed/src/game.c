#include <core/logger.h>

int main(void)
{
    StartupLogSystem
        (
         LOG_VERBOSITY_FLAG_ERROR | LOG_VERBOSITY_FLAG_WARNING |
         LOG_VERBOSITY_FLAG_SUCCESS | LOG_VERBOSITY_FLAG_INFO
        );

    LogError("Game", "Logging Error: %f", 3.14f);
    LogWarning("Game", "Logging Warning: %f", 3.14f);
    LogSuccess("Game", "Logging Success: %f", 3.14f);
    LogInfo("Game", "Logging Info: %f", 3.14f);

    ShutdownLogSystem();
}
