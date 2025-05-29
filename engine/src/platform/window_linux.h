#pragma once

#include "platform/window.h"
#include <X11/Xlib.h>

typedef struct LinuxWindow {
    Display* display;
    u32 screen;
    Window root;
    Window handle;
    XEvent event;
    Atom exitRequest;
} LinuxWindow;

LinuxWindow getLinuxWindow(const AppWindow window);
