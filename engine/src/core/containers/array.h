#pragma once

#include "defines.h"

#define array(TYPE, name)           \
    struct {                        \
        u32 dataSize;               \
        u32 length;                 \
        TYPE* data;                 \
    } name;                         \
                                    \
    name.dataSize = sizeof(TYPE);   \
    name.length = 0;                \
    name.data = NULL;

typedef struct general_array {
    u32 dataSize;
    u32 length;
    void* data;
} general_array;

#define array_ptr(array) \
    ((general_array*)&data)

void array_destroy(general_array* p_array);

void array_resize(general_array* p_array, const u32 length);
