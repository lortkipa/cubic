#pragma once

#include "defines.h"

typedef enum memory_tag {
    MEMORY_TAG_UNKNOWN,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_STRING,

    MEMORY_TAG_COUNT
} memory_tag;

typedef struct memory_tracker {
    bool isRunning;
    u64 allocated[MEMORY_TAG_COUNT];
    u64 freed[MEMORY_TAG_COUNT];
} memory_tracker;

const char* memory_tag_tostring(const memory_tag tag);

void memory_startup(void);

void memory_shutdown(void);

void* memory_allocate(const u32 size, const memory_tag tag);

void* memory_reallocate(void* memory, const u32 oldSize, const u32 size, const memory_tag tag);

void memory_free(void* memory, const u32 size, const memory_tag tag);
