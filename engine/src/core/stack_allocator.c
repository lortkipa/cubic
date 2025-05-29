#include "core/stack_allocator.h"
#include "core/assert.h"
#include <stdlib.h>

StackAllocator createStackAllocator(const u32 size)
{
    // check for incorrect params
    assert("Stack Allocator", size > 0, "can't create allocator with size of 0 bytes");

    // create allocator structure
    StackAllocator allocator;

    // init allocator 
    allocator.size = size;
    allocator.used = 0;

    // allocate memory
    allocator.memory = malloc(size);

    // return created allocator
    return allocator;
}

void destroyStackAllocator(StackAllocator* p_allocator)
{
    // check for incorrect params
    assert("Stack Allocator", p_allocator != null, "invalid pointer provided");

    // free memory
    free(p_allocator->memory);
}

void* requestStackAllocatorMemory(StackAllocator* p_allocator, const u32 size)
{
    // check for incorrect params
    assert("Stack Allocator", p_allocator != null, "invalid pointer provided");
    assert("Stack Allocator", size > 0, "can't request 0 bytes");

    //  save currently used bytes (before request)
    u32 used = p_allocator->used;

    // track allocation
    p_allocator->used += size;

    // return requested memory pointer
    return (void*)(p_allocator->memory + used);
}

void freeStackAllocatorMemory(StackAllocator* p_allocator, const u32 size)
{
    // check for incorrect params
    assert("Stack Allocator", p_allocator != null, "invalid pointer provided");
    assert("Stack Allocator", size > 0, "can't free 0 bytes");
    
    // track freeing
    p_allocator->used -= size;
}
