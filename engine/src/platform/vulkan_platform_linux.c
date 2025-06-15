#include "platform/vulkan_platform.h"

#if defined(PLATFORM_LINUX)

#include "platform/window_linux.h"
#include "core/logger.h"

#define CHANNEL "Vulkan Linux"

const char** GetVKInstanceExtensions(u8* p_count)
{
    static const char* exts[] = 
    {
        "VK_KHR_surface",
        "VK_KHR_xlib_surface",

#if defined(DEBUG)
        "VK_EXT_debug_utils"
#endif

    };

    // calculate ext count dynamcly
    *p_count = sizeof(exts) / sizeof(char*);

    // return filled exts array
    return exts;
}

b8 CreateVKSurface(const VkInstance instance, VkSurfaceKHR* p_surface)
{
    // surface info
    VkXlibSurfaceCreateInfoKHR surfaceInfo =
    {
        .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
        .dpy = GetLinuxWindowDisplay(),
        .window = GetLinuxWindowHandle()
    };

    // create surface
    VkResult result = vkCreateXlibSurfaceKHR(instance, &surfaceInfo, null, p_surface);

    // check if surface was created
    if (result != VK_SUCCESS)
    {
        LogError(CHANNEL, "Surface Not Created");
        return false;
    }

    LogSuccess(CHANNEL, "Surface Created");
    return true;
}

#endif
