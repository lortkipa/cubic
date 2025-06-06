#include "platform/window.h"
#include <stdint.h>

#if defined(PLATFORM_LINUX)

#include "platform/window_linux.h"
#include "core/logger.h"
#include "core/assert.h"
#include <X11/Xutil.h>

static bool initialized = false;
static LinuxWindow window;

bool CreateWindow(const u16 width, const u16 height, const char* title)
{
    // check params
    Assert("Window System", width > 0, "Can't Create Window With Width Of 0");
    Assert("Window System", height > 0, "Can't Create Window With Height Of 0");
    Assert("Window System", title != null, "Can't Create Window Title Of Null Pointer");

    // create display
    window.display = XOpenDisplay(null);

    // check for display creation error
    if (!window.display)
    {
        LogError("Window System", "X11 Display Creation Failed");
        return false;
    }

    // get screen
    window.screen = DefaultScreen(window.display);

    // get root window
    window.root = RootWindow(window.display, window.screen);

    // create window
    window.handle = XCreateSimpleWindow
        (
         window.display,
         window.root,
         0, 0,
         width, height, 
         0, 0, 0
        );
    
    // check for window creation error
    if (!window.handle)
    {
        LogError("Window System", "X11 Window Creation Failed");
        return false;
    }

    // set window title
    XStoreName(window.display, window.handle, title);

    // set window class
    XClassHint class = { (char*)title, WINDOW_CLASS };
    XSetClassHint(window.display, window.handle, &class);

    // set inputs
    XSelectInput(window.display, window.handle, 
            KeyPressMask | KeyReleaseMask | ExposureMask | ButtonPressMask | ButtonReleaseMask);

    // show the window
    XMapWindow(window.display, window.handle);

    // return success as default
    LogSuccess("Window System", "X11 Window Created %dx%d \"%s\"", width, height, title);
    initialized = true;
    return true;
}

void destroyWindow(void)
{
    // check if systme is Initialized
    Assert("Window System", initialized, "System Is Not Initialized");

    // destroy window
    XDestroyWindow(window.display, window.handle);
    LogSuccess("Window System", "Window Destroyed");

    // desroy display
    XCloseDisplay(window.display);

    // track that window is destroyed
    initialized = false;
}

void FireWindowEvents(void)
{
    // check if systme is Initialized
    Assert("Window System", initialized, "System Is Not Initialized");

    // continiue if there is upcoming events
    while (XPending(window.display))
    {
        // get next event
        XNextEvent(window.display, &window.event);
    }
}

LinuxWindow getLinuxWindow(void)
{
    // check if systme is Initialized
    Assert("Window System", initialized, "System Is Not Initialized");

    // return linux window structure
    return window;
}

#endif
