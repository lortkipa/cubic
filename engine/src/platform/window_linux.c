#include "platform/window.h"

#if defined(PLATFORM_LINUX)

#include "platform/window_linux.h"
#include "core/logger.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>

static LinuxWindow* windows = null;

bool createWindow(AppWindow* p_window, const u16 width, const u16 height, const char* title)
{
    // allocate windows array on heap if its not allocated yet
    if (!windows)
    {
        windows = malloc(MAX_WINDOW_COUNT * sizeof(LinuxWindow));
    }

    // look for emty space in windows array
    bool windowSpotFound = false;
    AppWindow window;
    for (u8 i = 0; i < MAX_WINDOW_COUNT; i++)
    {
        if (windows[i].display == null)
        {
            windowSpotFound = true;
            window = i;
            break;
        }
    }

    // if space for window is not found, return failure
    if (!windowSpotFound)
    {
        debugLogError("Linux Platform", "can't create anymore windows. maximum limit is reached");
        return false;
    }

    // create display
    windows[window].display = XOpenDisplay(null);

    // check if display was created
    if (windows[window].display == null)
    {
        debugLogError("Linux Platform", "window creation failed. can't create X11 display");
        return false;
    }

    // get screen id
    windows[window].screen = DefaultScreen(windows[window].display);

    // get root window
    windows[window].root = RootWindow(windows[window].display, windows[window].screen);

    // create window
    windows[window].handle = XCreateSimpleWindow 
        (
         windows[window].display,
         windows[window].root,
         0, 0,
         width, height,
         0, 0, 0
        );

    // set class
    XClassHint classHint = { (char*)title, WINDOW_CLASS_NAME };
    XSetClassHint(windows[window].display, windows[window].handle, &classHint);

    // set title
    XStoreName(windows[window].display, windows[window].handle, title);

    // set inputs
    XSelectInput
        (
         windows[window].display,
         windows[window].handle,
         ExposureMask | KeyPressMask | KeyReleaseMask
        );

    // show the window
    XMapWindow(windows[window].display, windows[window].handle);

    // if code comes here, everythings good, so return success
    debugLogSuccess("Linux Window", "window created %dx%d", width, height);
    *p_window = window;
    return true;
}

void destroyWindow(const AppWindow window)
{
    // destroy window
    XDestroyWindow(windows[window].display, windows[window].handle);
    debugLogSuccess("Linux Window", "window destroyed");

    // destroy connection
    XCloseDisplay(windows[window].display);
    windows[window].display = null;
}

void fireWindowEvents(const AppWindow window)
{
    // check if there is window events in the queue
    while (XPending(windows[window].display))
    {
        // get next event
        XNextEvent(windows[window].display, &windows[window].event);
    }
}

LinuxWindow getLinuxWindow(const AppWindow window)
{
    // return provided window
    return windows[window];
}

#endif
