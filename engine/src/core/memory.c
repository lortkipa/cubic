#include "core/memory.h"
#include "core/assert.h"
#include "core/logger.h"
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>

#define CHANNEL "Memory"

static MemoryTracker tracker = {};

b8 StartupMemorySystem(void)
{
    // make sure system is not started
    Assert(CHANNEL, tracker.isTracking == false, "System Is Already Initialized");

    // start tracking
    tracker.isTracking = true;
    LogSuccess(CHANNEL, SYSTEM_INITIALIZED_MESSAGE);

    // return true as default
    return true;
}

void ShutdownMemorySystem(void)
{
    // make sure system is started
    Assert(CHANNEL, tracker.isTracking == true, SYSTEM_NOT_INITIALIZED_MESSAGE);

    // stop tracking
    tracker.isTracking = false;
    LogSuccess(CHANNEL, SYSTEM_TERMINATED_MESSAGE);
    LogInfo(CHANNEL, "Total Allocated: %dB, Freed: %dB, Usage: %dB",
            tracker.allocated, tracker.freed, tracker.usage);
}

void* AllocateMemory(const u32 size)
{
    // make sure system is started
    Assert(CHANNEL, tracker.isTracking == true, SYSTEM_NOT_INITIALIZED_MESSAGE);
    
    // make sure size is not zero
    Assert(CHANNEL, size > 0, "Invalid Memory Size Provided");

    // allocate memory on heap
    void* newMemory = malloc(size);

    // track allocation if there are no errors
    if (newMemory)
    {
        tracker.allocated += size;
        tracker.usage += size;
        LogSuccess(CHANNEL, "Allocated %dB Bytes On Heap", size);
    }
    else 
    {
        LogWarning(CHANNEL, "Failed To Allocate %dB Bytes On Heap", size);
    }

    // return allocated memory
    return newMemory;
}

void FreeMemory(void* memory, const u32 size)
{
    // make sure system is started
    Assert(CHANNEL, tracker.isTracking == true, SYSTEM_NOT_INITIALIZED_MESSAGE);
    
    // make sure size is not zero
    Assert(CHANNEL, size > 0, "Invalid Memory Size Provided");

    // make sure memory is not null
    Assert(CHANNEL, memory != null, "Invalud Memory Pointer Providede");

    // free memory
    free(memory);

    // track freeing
    tracker.freed += size;
    tracker.usage -= size;
}

u32 GetCurrentMemoryUsage(void)
{
    // make sure system is started
    Assert(CHANNEL, tracker.isTracking == true, SYSTEM_NOT_INITIALIZED_MESSAGE);

    // return memory usage in bytes
    return tracker.usage;
}
