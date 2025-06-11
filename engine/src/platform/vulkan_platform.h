#pragma once

#include "defines.h"
#include <renderer/vulkan_renderer.h>

const char** GetRequiredVKInstanceExtensions(u8* p_count, const b8 enableDebugUtils);

VkResult CreateVKSurface(VKRenderer* p_renderer);
