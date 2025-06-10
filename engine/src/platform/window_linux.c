#include "platform/window.h"

#if defined(PLATFORM_LINUX)

#define CHANNEL "Linux Window"

#include "platform/window_linux.h"
#include "core/assert.h"
#include "core/logger.h"
#include "core/memory.h"
#include <X11/Xutil.h>

static LinuxWindow* window = null;

EXPORT b8 CreateWindow(const u16 width, const u16 height, const char* title)
{
    // allocate structure on heap
    window = AllocateMemory(sizeof(Window));

    // check if singleton was allocated
    if (!window)
    {
        LogError(CHANNEL, "Singleton Allocation Failed");
        return false;
    }

    // create display
    window->display = XOpenDisplay(null);

    // check if display was created
    if (!window->display)
    {
        LogError(CHANNEL, "Display Creation Failed");
        return false;
    }

    // get screen
    window->screen = DefaultScreen(window->display);

    // get root window
    window->root = RootWindow(window->display, window->screen);

    // create window
    window->handle = XCreateSimpleWindow
        (
         window->display, 
         window->root, 
         0, 0, 
         width, height, 
         0, 0, 0
         );

    // check if window was created
    if (!window->handle)
    {
        LogError(CHANNEL, "Window Creation Failed");
        return false;
    }

    // set title
    XStoreName(window->display, window->handle, title);

    // set class
    XClassHint class = { (char*)title, WINDOW_CLASS };
    XSetClassHint(window->display, window->handle, &class);

    // select window inputs
    XSelectInput(window->display, window->handle, 
            ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask);

    // init exit request
    window->exitRequest = XInternAtom(window->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(window->display, window->handle, &window->exitRequest, 1);

    // show the window
    XMapWindow(window->display, window->handle);

    // return success
    LogSuccess(CHANNEL, "Window Created %dx%d \"%s\"", width, height, title);
    return true;
}

EXPORT void DestroyWindow(void)
{
    // destroy window
    XDestroyWindow(window->display, window->handle);
    LogSuccess(CHANNEL, "Window Destroyed");

    // destroy display
    XCloseDisplay(window->display);
}

EXPORT void FireWindowEvents(void)
{
    // check if there is upcoming events
    while (XPending(window->display))
    {
        // get next event
        XNextEvent(window->display, &window->event);
    }
}

Display* GetLinuxWindowDisplay(void)
{
    return window->display;
}

u32 GetLinuxWindowScreen(void)
{
    return window->screen;
}

Window GetLinuxWindowRoot(void)
{
    return window->root;
}

Window GetLinuxWindowHandle(void)
{
    return window->handle;
}

XEvent GetLinuxWindowEvent(void)
{
    return window->event;
}

#endif
