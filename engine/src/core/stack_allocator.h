#pragma once

#include "defines.h"

typedef struct StackAllocator {
    u32 size;
    u32 used;
    void* memory;
} StackAllocator;

b8 CreateStackAllocator(StackAllocator* p_allocator, const u32 size);

void DestroyStackAllocator(StackAllocator* p_allocator);

void* RequestStackAllocatorMemory(StackAllocator* p_allocator, const u32 size);

void FreeStackAllocatorMemory(StackAllocator* p_allocator, const u32 size);
