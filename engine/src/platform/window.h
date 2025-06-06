#pragma once

#include "defines.h"

#define WINDOW_CLASS "Cubic Engine"

bool CreateWindow(const u16 width, const u16 height, const char* title);

void destroyWindow(void);

void FireWindowEvents(void);
