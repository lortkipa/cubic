#pragma once

#include "defines.h"

#define MAX_WINDOW_COUNT 5
#define WINDOW_CLASS_NAME "Cubic Engine"

typedef u8 AppWindow;

bool createWindow(AppWindow* p_window, const u16 width, const u16 height, const char* title);

void destroyWindow(const AppWindow window);

void fireWindowEvents(const AppWindow window);
