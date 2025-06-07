#include "core/stack_allocator.h"
#include "core/memory.h"
#include "core/logger.h"
#include "core/assert.h"

StackAllocator CreateStackAllocator(const u32 size)
{
    // check if correct size is provided
    Assert("Stack Allocator", size > 0, "Can't Create Allocator With Size Of 0B");

    // create structure
    StackAllocator allocator;
    
    // zero out stuff
    allocator.memory = null;
    allocator.size = 0;
    allocator.topMarker = 0;

    // allocate memory
    allocator.memory = AllocateMemory(size);

    // check if memory was allocated
    if (!allocator.memory)
    {
        // return allocator before its size is initialized
        LogError("Stack Allocator", "Failed To Create Allocator Of Size %dB", size);
        return allocator;
    }

    // save size
    allocator.size = size;

    // return created allocator
    LogSuccess("Stack Allocator", "Allocator Created With Size Of %dB", size);
    return allocator;
}

void DestroyStackAllocator(StackAllocator* p_allocator)
{
    // check for valid allocator
    Assert("Stack Allocator", p_allocator != null, "Invalid Allocator Provided");

    // free memory
    FreeMemory(p_allocator->memory, p_allocator->size);
    LogSuccess("Stack Allocator", "Allocator With Size of %dB Destroyed", p_allocator->size);

    // zero out stuff
    p_allocator->memory = null;
    p_allocator->size = 0;
    p_allocator->topMarker = 0;
}

StackAllocatorMarker GetMarker(const StackAllocator* p_allocator)
{
    // check for valid allocator
    Assert("Stack Allocator", p_allocator != null, "Invalid Allocator Provided");

    // return current marker
    return p_allocator->topMarker;
}

void* RequestStackAllocatorMemory(StackAllocator* p_allocator, const u32 size)
{
    // check if correct size is provided
    Assert("Stack Allocator", size > 0, "0B Requested");

    // check for valid allocator
    Assert("Stack Allocator", p_allocator != null, "Invalid Allocator Provided");

    // check if too much memory is requested
    Assert("Stack Allocator", p_allocator->topMarker + size <= p_allocator->size, "Too Much Memory Requested");

    // move marker
    p_allocator->topMarker += size;

    // return requested memory
    return (char*)p_allocator->memory + (p_allocator->topMarker - size);
}

void FreeStackAllocatorMemory(StackAllocator* p_allocator, const u32 size)
{
    // check if correct size is provided
    Assert("Stack Allocator", size > 0, "0B Requested");

    // check for valid allocator
    Assert("Stack Allocator", p_allocator != null, "Invalid Allocator Provided");

    // move back the marker
    p_allocator->topMarker -= size;
}

void FreeStackAllocatorMemoryToMarker(StackAllocator* p_allocator, const StackAllocatorMarker marker)
{
    // check for valid allocator
    Assert("Stack Allocator", p_allocator != null, "Invalid Allocator Provided");

    // check if marker is in correct bounds
    Assert("Stack Allocator", marker > 0 && marker <= p_allocator->topMarker, "Invalid Marker Provided");

    // move back the marker
    p_allocator->topMarker = marker;
}

void ClearStackAllocatorMemory(StackAllocator* p_allocator)
{
    // check for valid allocator
    Assert("Stack Allocator", p_allocator != null, "Invalid Allocator Provided");

    // move back marker to the begining
    p_allocator->topMarker = 0;
}
