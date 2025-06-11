#pragma once

#include "defines.h"

typedef enum RendererBackend {
    RENDERER_BACKEND_VULKAN,
    RENDERER_BACKEND_DIRECX12
} RendererBackend;

typedef struct Renderer {
    RendererBackend Backend;

    b8 (*Startup)(void);
    void (*Shutdown)(void);
    void (*Draw)(void);
} Renderer;

b8 StartupRenderer(const RendererBackend backend);

void ShutdownRenderer(void);

void RunRenderer(void);
