#include "platform/window.h"

#if defined(PLATFORM_LINUX)

#define CHANNEL "Linux Window"

#include "platform/window_linux.h"
#include "core/assert.h"
#include "core/logger.h"
#include "core/memory.h"
#include "core/event.h"
#include <X11/Xutil.h>

static LinuxWindow* window = null;

EXPORT b8 CreateWindow(const u16 width, const u16 height, const char* title)
{
    // allocate structure on heap
    window = AllocateMemory(sizeof(LinuxWindow));

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
    
    // free allocated memory
    FreeMemory(window, sizeof(LinuxWindow));
}

EXPORT void FireWindowEvents(void)
{
    // check if there is upcoming events
    while (XPending(window->display))
    {
        // get next event
        XNextEvent(window->display, &window->event);

        // if window resized, store updated size here
        XWindowAttributes attributes;

        // look at events and fire them
        switch (window->event.type) 
        {
            case KeyPress:
                FireEvent(EVENT_TYPE_KEY_PRESS);
                SetEventArgI32(EVENT_TYPE_KEY_PRESS, 0, "Key", window->event.xkey.keycode);
                break;
            case KeyRelease:
                FireEvent(EVENT_TYPE_KEY_RELEASE);
                SetEventArgI32(EVENT_TYPE_KEY_RELEASE, 0, "Key", window->event.xkey.keycode);
                break;
            case Expose:
                XGetWindowAttributes(window->display, window->handle, &attributes);
                FireEvent(EVENT_TYPE_WINDOW_RESIZE);
                SetEventArgI32(EVENT_TYPE_WINDOW_RESIZE, 0, "Width", attributes.width);
                SetEventArgI32(EVENT_TYPE_WINDOW_RESIZE, 1, "Height", attributes.height);
                break;
            case ClientMessage:
                if (window->event.xclient.data.l[0] == (u32)window->exitRequest)
                {
                    FireEvent(EVENT_TYPE_WINDOW_EXIT_REQUEST);
                }
                break;
        }
    }
}

EXPORT WindowSize GetWindowSize(void)
{
    // store window attributes here (temp buffer)
    XWindowAttributes attributes;

    // get window attributes
    XGetWindowAttributes(window->display, window->handle, &attributes);

    // create structure
    WindowSize size = { attributes.width, attributes.height };

    // return window size
    return size;
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
