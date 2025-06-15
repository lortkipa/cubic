#pragma once

#include "renderer/renderer_defines.h"
#include <vulkan/vulkan.h>

typedef struct VKRenderer {
    VkInstance Instance;

#if defined(DEBUG)
    VkDebugUtilsMessengerEXT DebugMessenger;
#endif

    VkSurfaceKHR Surface;
    VkPhysicalDevice GPU;
    u32 graphicsQueueIndex;
    VkDevice Device;
} VKRenderer;

b8 StartupVKRenderer(void);

void ShutdownVKRenderer(void);

void DrawVKFrame(void);
