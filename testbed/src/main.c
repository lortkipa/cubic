#include <core/logger.h>
#include <core/memory.h>
#include <core/stack_allocator.h>

int main(void)
{
    // startup systems
    StartupLogSystem(LOG_VERBOSITY_FLAG_ERROR | LOG_VERBOSITY_FLAG_WARNING | LOG_VERBOSITY_FLAG_SUCCESS | LOG_VERBOSITY_FLAG_INFO);
    StartupMemorySystem();

    StackAllocator allocator;
    CreateStackAllocator(&allocator, 2 * sizeof(int) + 3 * sizeof(float));

    int* data = RequestStackAllocatorMemory(&allocator, 2 * sizeof(int));
    data[0] = 14;
    data[1] = 16;
    FreeStackAllocatorMemory(&allocator, 2 * sizeof(int));
    float* PIs = RequestStackAllocatorMemory(&allocator, 3 * sizeof(float));
    PIs[0] = 3.14;
    PIs[1] = 3.1414;
    PIs[2] = 3.14;
    for (int i = 0; i < 2; i++)
        LogInfo("Game", "%d. %d", i+1, data[i]);
    for (int i = 0; i < 3; i++)
        LogInfo("Game", "%d. %f", i+1, PIs[i]);

    DestroyStackAllocator(&allocator);

    // shutdown systems
    ShutdownMemorySystem();
    ShutdownLogSystem();
}
