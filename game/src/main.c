#include <core/memory/memory.h>

int main(void)
{
    memory_startup();

    u32* data = memory_allocate(2 * sizeof(u32), MEMORY_TAG_ARRAY);
    data = memory_reallocate(data, 2 * sizeof(u32), 3 * sizeof(u32), MEMORY_TAG_ARRAY);
    memory_free(data, 3 * sizeof(u32), MEMORY_TAG_ARRAY);

    memory_shutdown();
}
