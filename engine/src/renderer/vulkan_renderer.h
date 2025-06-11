#pragma once

#include "defines.h"

#include <vulkan/vulkan.h>

typedef struct VKQueueFamilyIndinces {
    i32 graphics;
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
