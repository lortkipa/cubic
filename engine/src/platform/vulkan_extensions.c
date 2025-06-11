#include "platform/vulkan_extensions.h"
#include "core/logger.h"

#define CHANNEL "Vulkan Extension Manager"

const char** GetRequiredVKInstanceExtensions(u8* p_count, const b8 enableDebugUtils)
{
#if defined(PLATFORM_LINUX)

    // windows extensions
    static const char* extensions[] = 
    {
        "VK_KHR_surface",
        "VK_KHR_xlib_surface",
        "VK_EXT_debug_utils"
    };

#elif defined(PLATFORM_WINDOWS)

    // windows extensions
    static const char* extensions[] = 
    {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
        "VK_EXT_debug_utils"
    };
   

#else 

    // asign and return 0 if platform is not found
    LogWarning(CHANNEL, "Vulkan Extensions Not Supported For Current Platform");
    *p_count = 0;
    return null;

#endif

    // get and asign size
    *p_count = sizeof(extensions) / sizeof(char*);

    // if debug utils is not enabled, we have to remove last extensions, so just resize ext count
    if (!enableDebugUtils)
    {
        (*p_count)--;
    }

    // return extensions
    return extensions;
}
