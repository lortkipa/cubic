#include <platform/window.h>
#include <core/logger.h>
#include <core/assert.h>
#include <core/memory.h>
#include <core/stack_allocator.h>

int main(void)
{
    StartupLogSystem
        (
         LOG_VERBOSITY_FLAG_ERROR | LOG_VERBOSITY_FLAG_WARNING |
         LOG_VERBOSITY_FLAG_SUCCESS | LOG_VERBOSITY_FLAG_INFO
        );
    StartupMemorySystem();

    StackAllocator allocator = CreateStackAllocator(7 * sizeof(float));
    int* ages = RequestStackAllocatorMemory(&allocator, 2 * sizeof(int));
    StackAllocatorMarker agesMarker = GetMarker(&allocator);
    FreeStackAllocatorMemoryToMarker(&allocator, agesMarker);
    float* PIs = RequestStackAllocatorMemory(&allocator, 5 * sizeof(float));
    ages[0] = 14;
    ages[1] = 16;
    PIs[0] = 3.14;
    PIs[1] = 3.14;
    PIs[2] = 3.14;
    PIs[3] = 3.14;
    PIs[4] = 3.14;
    for (int i = 0; i < 2; i++)
        LogInfo("Game", "%d. %d", i + 1, ages[i]);
    for (int i = 0; i < 5; i++)
        LogInfo("Game", "%d. %f", i + 1, PIs[i]);
    DestroyStackAllocator(&allocator);

    // CreateWindow(1000, 800, "Cubic Engine");
    // while (true)
    // {
    //     FireWindowEvents();
    // }
    // destroyWindow();

    ShutdownMemorySystem();
    ShutdownLogSystem();
}
