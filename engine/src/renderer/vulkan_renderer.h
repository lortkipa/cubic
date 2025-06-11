#pragma once

#include "defines.h"

#include <vulkan/vulkan.h>

typedef struct VKRenderer {
    VkInstance instance;
    VkDebugUtilsMessengerEXT messenger;
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    VkSurfaceKHR surface;
} VKRenderer;

b8 StartupVKRenderer(void);

void ShutdownVKRenderer(void);

void DrawVKRenderer(void);
