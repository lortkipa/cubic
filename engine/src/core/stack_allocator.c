#include "core/stack_allocator.h"
#include "core/memory.h"
#include "core/assert.h"
#include "core/logger.h"

#define CHANNEL "Stack Allocator"

b8 CreateStackAllocator(StackAllocator* p_allocator, const u32 size)
{
    // check for invalid pointers
    Assert(CHANNEL, p_allocator != null, "Invalid Pointer Provided");

    // make sure size is greater than 0
    Assert(CHANNEL, size > 0, "Invalid Size Provided");

    // allocate memory on heap
    p_allocator->memory = AllocateMemory(size);

    // check for allocation errors
    if (!p_allocator->memory)
    {
        LogError(CHANNEL, "Memory Allocation Failed");
        return false;
    }

    // init attributes
    p_allocator->size = size;
    p_allocator->used = 0;

    // return success as default
    LogSuccess(CHANNEL, "Allocator Created");
    return true;
}

void DestroyStackAllocator(StackAllocator* p_allocator)
{
    // check for invalid pointers
    Assert(CHANNEL, p_allocator != null, "Invalid Pointer Provided");
    Assert(CHANNEL, p_allocator->memory != null, "Invalid Pointer Provided");

    // free memory
    FreeMemory(p_allocator->memory, p_allocator->size);
    LogSuccess(CHANNEL, "Allocator Destroyed With Size %dB", p_allocator->size);

    // zero out stuff
    p_allocator->memory = null;
    p_allocator->used = 0;
    p_allocator->size = 0;
}

void* RequestStackAllocatorMemory(StackAllocator* p_allocator, const u32 size)
{
    // check for invalid pointers
    Assert(CHANNEL, p_allocator != null, "Invalid Pointer Provided");
    Assert(CHANNEL, p_allocator->memory != null, "Allocator Is Not Created Yet");

    // make sure size is greater than 0
    Assert(CHANNEL, size > 0, "Invalid Size Provieded");
    
    // make sure allocator is able to allocate that much memory
    Assert(CHANNEL, size <= (p_allocator->size - p_allocator->used), "Too Much Memory Requested");

    // move marker
    p_allocator->used += size;

    // return requested memory
    return (char*)p_allocator->memory + p_allocator->used - size;
}

void FreeStackAllocatorMemory(StackAllocator* p_allocator, const u32 size)
{
    // check for invalid pointers
    Assert(CHANNEL, p_allocator != null, "Invalid Pointer Provided");
    Assert(CHANNEL, p_allocator->memory != null, "Allocator Is Not Created Yet");

    // make sure size is greater than 0
    Assert(CHANNEL, size > 0, "Invalid Size Provided");
    
    // make sure allocator is able to free that much memory
    Assert(CHANNEL, size <= (p_allocator->size - p_allocator->used), "Too Much Memory Requested");

    // move marker
    p_allocator->used -= size;
}
