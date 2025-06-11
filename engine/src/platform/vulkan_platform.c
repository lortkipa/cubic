#include "platform/vulkan_platform.h"
#include "platform/window_linux.h"
#include "core/logger.h"

#define CHANNEL "Vulkan Platform"

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

VkResult CreateVKSurface(VKRenderer* p_renderer)
{
#if defined(PLATFORM_LINUX)

    // linux surface info
    VkXlibSurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.dpy = GetLinuxWindowDisplay();
    surfaceInfo.window = GetLinuxWindowHandle();

    // create surface and return result
    return vkCreateXlibSurfaceKHR(p_renderer->instance, &surfaceInfo, null, &p_renderer->surface);

#elif defined(PLATFORM_WINDOWS)

#endif
}
