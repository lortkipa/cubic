#include "core/stack_allocator.h"
#include "core/logger.h"

int main(void)
{
    StackAllocator allocator = createStackAllocator(2000);

    int* ages = requestStackAllocatorMemory(&allocator, 2 * sizeof(int));
    float* pi = requestStackAllocatorMemory(&allocator, 5 * sizeof(float));

    ages[0] = 14;
    ages[1] = 17;
    pi[0] = 3.14;
    pi[1] = 3.14;
    pi[2] = 3.14;
    pi[3] = 3.14;
    pi[4] = 3.14;

    for (fu32 i = 0; i < 2; i++)
    {
        debugLogInfo("Game", "%d. %d", i+1, ages[i]);
    }

    for (fu32 i = 0; i < 3; i++)
    {
        debugLogInfo("Game", "%d. %f", i+1, pi[i]);
    }

    destroyStackAllocator(&allocator);

    return 0;
}
