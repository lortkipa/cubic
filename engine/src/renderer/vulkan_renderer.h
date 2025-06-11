#pragma once

#include "defines.h"

#if defined(PLATFORM_LINUX)
    #define VK_USE_PLATFORM_XLIB_KHR
#elif defined(PLATFORM_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

typedef struct VKQueueFamilyIndinces {
    u32 graphics;
    u32 present;
} VKQueueFamilyIndinces;

typedef struct VKQueueHandles {
    VkQueue graphics;
} VKQueueHandles;

typedef struct VKRenderer {
    VkInstance instance;
    VkDebugUtilsMessengerEXT messenger;
    VKQueueFamilyIndinces queueFamilyIndinces;
    VKQueueHandles queueHandles;
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    VkSurfaceKHR surface;
} VKRenderer;

b8 StartupVKRenderer(void);

void ShutdownVKRenderer(void);

void DrawVKRenderer(void);
