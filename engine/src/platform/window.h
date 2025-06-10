#pragma once

#include "defines.h"

#define WINDOW_CLASS "Cubic Engine"

EXPORT b8 CreateWindow(const u16 width, const u16 height, const char* title);

EXPORT void DestroyWindow(void);

EXPORT void FireWindowEvents(void);
