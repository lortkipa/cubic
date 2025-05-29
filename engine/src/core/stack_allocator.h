#pragma once

#include "defines.h"

typedef struct StackAllocator {
    void* memory;
    u32 size;
    u32 used;
} StackAllocator;

StackAllocator createStackAllocator(const u32 size);

void destroyStackAllocator(StackAllocator* p_allocator);

void* requestStackAllocatorMemory(StackAllocator* p_allocator, const u32 size);

void freeStackAllocatorMemory(StackAllocator* p_allocator, const u32 size);
