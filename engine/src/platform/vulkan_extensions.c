#include "platform/vulkan_extensions.h"
#include "core/logger.h"

#define CHANNEL "Vulkan Extension Manager"

const char** GetRequiredVKInstanceExtensions(u8* p_count)
{
#if defined(PLATFORM_LINUX)

    // windows extensions
    static const char* extensions[] = 
    {
        "VK_KHR_surface",
        "VK_KHR_xlib_surface"
    };

#elif defined(PLATFORM_WINDOWS)

    // windows extensions
    static const char* extensions[] = 
    {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };
   

#else 

    // asign and return 0 if platform is not found
    LogWarning(CHANNEL, "Vulkan Extensions Not Supported For Current Platform");
    *p_count = 0;
    return null;

#endif

    // get and asign size
    *p_count = sizeof(extensions) / sizeof(char*);

    // return extensions
    return extensions;
}
