#include "platform/vulkan_platform.h"

const char** GetVKInstanceExtensions(u8* p_count)
{
    // define extensions array depending on platform
#if defined(PLATFORM_LINUX)

    static const char* exts[] = 
    {
        "VK_KHR_surface",
        "VK_KHR_xlib_surface",

#if defined(DEBUG)
        "VK_EXT_debug_utils"
#endif

    };

#elif defined(PLATFORM_WINDOWS)

    static const char* exts[] = 
    {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",

#if defined(DEBUG)
        "VK_EXT_debug_utils"
#endif

    };

#endif

    // calculate ext count dynamcly
    *p_count = sizeof(exts) / sizeof(char*);

    // return filled exts array
    return exts;
}
