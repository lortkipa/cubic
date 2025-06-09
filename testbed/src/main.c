#include <core/assert.h>
#include <core/logger.h>

int main(void)
{
    // startup systems
    StartupLogSystem(LOG_VERBOSITY_FLAG_ERROR | LOG_VERBOSITY_FLAG_WARNING | LOG_VERBOSITY_FLAG_SUCCESS | LOG_VERBOSITY_FLAG_INFO);

    Assert("Game", 1 == 1, "Shit! App Failed!");
    Assert("Game", 1 == 2, "Shit! App Failed!");

    // shutdown systems
    ShutdownLogSystem();
}
