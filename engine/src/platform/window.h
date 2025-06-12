#pragma once

#include "defines.h"

#define WINDOW_CLASS "Cubic Engine"

typedef struct WindowSize {
    u16 width;
    u16 height;
} WindowSize;

EXPORT b8 CreateWindow(const u16 width, const u16 height, const char* title);

EXPORT void DestroyWindow(void);

EXPORT void FireWindowEvents(void);

EXPORT WindowSize GetWindowSize(void);
