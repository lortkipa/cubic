#pragma once

#include "defines.h"

typedef struct MemoryTracker {
    bool isRunning;
    u32 allocated;
    u32 freed;
    u32 usage;
} MemoryTracker;

bool StartupMemorySystem(void);

void ShutdownMemorySystem(void);

void* AllocateMemory(const u32 size);

void FreeMemory(void* memory, const u32 size);
