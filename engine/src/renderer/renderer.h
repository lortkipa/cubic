#pragma once

#include "defines.h"

typedef struct Color {
    i8 r;
    i8 g;
    i8 b;
    i8 a;
} Color;

typedef struct Renderer {
    Color background;
} Renderer;

b8 StartupRenderer(void);

void ShutdownRenderer(void);

void DrawFrame(void);
