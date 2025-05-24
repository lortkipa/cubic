#include <core/memory/memory.h>
#include <core/containers/array.h>
#include <stdio.h>

int main(void)
{
    memory_startup();

    array(u32, data);
    array_resize(array_ptr(data), 2);
    array_resize(array_ptr(data), 4);
    array_destroy(array_ptr(data));

    memory_shutdown();
}
