#pragma once

#include "defines.h"

#if defined(PLATFORM_LINUX)
    #define VK_USE_PLATFORM_XLIB_KHR
#elif defined(PLATFORM_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>

const char** GetVKInstanceExtensions(u8* p_count);

b8 CreateVKSurface(const VkInstance instance, VkSurfaceKHR* p_surface);
