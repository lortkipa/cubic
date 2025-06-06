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

    int* data = AllocateMemory(2 * sizeof(int));
    data[0] = 100;
    data[1] = 200;
    for (int i = 0; i < 2; i++)
    {
        LogInfo("Game", "%d. %d", i+1, data[i]);
    }

    FreeMemory(data, 2 * sizeof(int));

    ShutdownMemorySystem();
    ShutdownLogSystem();
}
