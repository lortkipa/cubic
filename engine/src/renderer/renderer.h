#pragma once

#include "renderer/renderer_defines.h"

typedef enum RendererBackend {
    RENDERER_BACKEND_VULKAN
} RendererBackend;

typedef struct Renderer {
    b8 (*Startup)(void);
    void (*Shutdown)(void);
    void (*DrawFrame)(void);

    Color background;
} Renderer;

b8 StartupRenderer(const RendererBackend backend);

void ShutdownRenderer(void);

void DrawFrame(void);
