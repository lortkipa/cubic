#include "core/memory/memory.h"

#include "core/debug/assert.h"
#include "core/debug/logger.h"

#include <stdlib.h>

static memory_tracker tracker = {};

const char* memory_tag_tostring(const memory_tag tag)
{
    switch (tag) 
    {
        case MEMORY_TAG_UNKNOWN:
            return "Unknown";
            break;
        case MEMORY_TAG_ARRAY:
            return "Array";
            break;
        case MEMORY_TAG_STRING:
            return "String";
            break;
        default:
            debugLogWarning("memory", "invalid memory tag provided to memory_tag_tostring()");
            break;
    }
    return "";
}

void memory_startup(void)
{
    // start system
    tracker.isRunning = true;
    debugLogSuccess("memory", "system initialized");

    // zero out everything
    for (fu8 i = 0; i < MEMORY_TAG_COUNT; i++)
    {
        tracker.allocated[i] = 0;
        tracker.freed[i] = 0;
    }
}

void memory_shutdown(void)
{
    // check if system is started before shutdown
    assert("memory", tracker.isRunning, "can not shutdown system without startup");

    // stop system
    tracker.isRunning = false;
    debugLogSuccess("memory", "system terminated");

    // log memory use
    debugLogInfo("memory", "---------- use ----------");
    for (fu32 i = 0; i < MEMORY_TAG_COUNT; i++)
    {
        debugLogInfo("memory", "%s: %dB || %dB",
                memory_tag_tostring(i), tracker.allocated[i], tracker.freed[i]);
    }
}

void* memory_allocate(const u32 size, const memory_tag tag)
{
    // check if correct values are provided
    assert("memory", size != 0, "new size can't be size of 0 bytes");

    // allocate memory
    void* newMemory = malloc(size);

    // check for errors
    if (!newMemory)
    {
        debugLogError("memory", "allocation failed");
    }
    else 
    {
        // track memory manipulation
        tracker.allocated[tag] += size;
    }

    // return allocated memory
    return newMemory;
}

void* memory_reallocate(void* memory, const u32 oldSize, const u32 size, const memory_tag tag)
{
    // check if correct values are provided
    assert("memory", memory != NULL, "invalid memory pointer provided");
    assert("memory", oldSize != 0, "old size can't be size of 0 bytes");
    assert("memory", size != 0, "new size can't be size of 0 bytes");

    // reallocate memory
    void* newMemory = realloc(memory, size);

    // check for errors
    if (!newMemory)
    {
        debugLogError("memory", "reallocation failed");
    }
    else 
    {
        // track memory manipulation
        tracker.allocated[tag] += size - oldSize;
    }

    // return reallocated memory
    return newMemory;
}

void memory_free(void* memory, const u32 size, const memory_tag tag)
{
    // check if correct values are provided
    assert("memory", memory != NULL, "invalid memory pointer provided");
    assert("memory", size != 0, "can't free memory with size of 0 bytes");

    // free memory
    free(memory);

    // track memory manipulation
    tracker.freed[tag] += size;
}
