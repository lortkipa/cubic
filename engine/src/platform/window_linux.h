#pragma once

#include "defines.h"
#include <X11/Xlib.h>

typedef struct LinuxWindow {
    Display* display;
    u32 screen;
    Window root;
    Window handle;
    Atom exitRequest;
    XEvent event;
} LinuxWindow;

Display* GetLinuxWindowDisplay(void);

u32 GetLinuxWindowScreen(void);

Window GetLinuxWindowRoot(void);

Window GetLinuxWindowHandle(void);

XEvent GetLinuxWindowEvent(void);
