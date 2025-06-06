#include "core/memory.h"
#include "core/assert.h"
#include "core/logger.h"
#include <stdlib.h>

static MemoryTracker tracker = {};

bool StartupMemorySystem(void)
{
    // start system
    tracker.isRunning = true;

    // zero out stuff
    tracker.allocated = 0;
    tracker.freed = 0;
    tracker.usage = 0;

    // return success as defualt
    LogSuccess("Memory System", "System Initialized");
    return true;
}

void ShutdownMemorySystem(void)
{
    // check if system is initialized
    Assert("Memory System", tracker.isRunning, "System Is Not Initialized");

    // stop system
    tracker.isRunning = false;
    LogSuccess("Memory System", "System Terminated");

    // log stats
    LogInfo("Memory System", "Allocated: %dB. Freed: %dB. Usage: %dB",
            tracker.allocated, tracker.freed, tracker.usage);
}

void* AllocateMemory(const u32 size)
{
    // check if system is initialized
    Assert("Memory System", tracker.isRunning, "System Is Not Initialized");

    // check params
    Assert("Memory System", size > 0, "Can't Allocate Memory With Size Of 0B");

    // allocate memory
    void* memory = malloc(size);

    // check for errors
    if (memory == null)
    {
        LogError("Memory System", "Allocation Failed");
    }
    else
    {
        LogSuccess("Memory System", "Allocated %dB", size);

        // track allocation
        tracker.allocated += size;
        tracker.usage += size;
    }

    // return allocated memory
    return memory;
}

void FreeMemory(void* memory, const u32 size)
{
    // check if system is initialized
    Assert("Memory System", tracker.isRunning, "System Is Not Initialized");

    // check params
    Assert("Memory System", memory != null, "Can't Free Null Pointer Memory");
    Assert("Memory System", size > 0, "Can't Free Memory With Size Of 0B");

    // free the memory
    free(memory);
    
    // track freeing
    tracker.freed += size;
    tracker.usage -= size;
    LogSuccess("Memory System", "Freed %dB", size);
}
