#pragma once

#include "defines.h"

typedef u32 StackAllocatorMarker;

typedef struct StackAllocator {
    void* memory;
    StackAllocatorMarker topMarker;
    u32 size;
} StackAllocator;

StackAllocator CreateStackAllocator(const u32 size);

void DestroyStackAllocator(StackAllocator* p_allocator);

StackAllocatorMarker GetStackAllocatorMarker(const StackAllocator* p_allocator);

void* RequestStackAllocatorMemory(StackAllocator* p_allocator, const u32 size);

void FreeStackAllocatorMemory(StackAllocator* p_allocator, const u32 size);

void FreeStackAllocatorMemoryToMarker(StackAllocator* p_allocator, const StackAllocatorMarker marker);

void ClearStackAllocatorMemory(StackAllocator* p_allocator);
