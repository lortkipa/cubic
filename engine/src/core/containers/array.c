#include "core/containers/array.h"

#include "core/memory/memory.h"

void array_destroy(general_array* p_array)
{
    // free data
    memory_free(
            p_array->data,
            p_array->length * sizeof(p_array->dataSize),
            MEMORY_TAG_ARRAY);

    // zero out array members
    p_array->data = NULL;
    p_array->length = 0;
}

void array_resize(general_array* p_array, const u32 length)
{
    // allocate or reallocate depending on situation
    if (p_array->data == NULL)
    {
        p_array->data = memory_allocate(
                length * sizeof(p_array->dataSize),
                MEMORY_TAG_ARRAY);
    }
    else 
    {
        p_array->data = memory_reallocate(
                p_array->data,
                p_array->length * sizeof(p_array->dataSize),
                length * sizeof(p_array->dataSize),
                MEMORY_TAG_ARRAY);
    }

    // store new length
    p_array->length = length;
}
