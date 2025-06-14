#pragma once

#include "defines.h"

typedef struct MemoryTracker {
    b8 isTracking;
    u32 allocated;
    u32 freed;
    u32 usage;
} MemoryTracker;

EXPORT b8 StartupMemorySystem(void);

EXPORT void ShutdownMemorySystem(void);

void* AllocateMemory(const u32 size);

void FreeMemory(void* memory, const u32 size);

u32 GetCurrentMemoryUsage(void);
