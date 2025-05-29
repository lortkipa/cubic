#include <platform/window.h>

int main(void)
{
    AppWindow window;
    if (createWindow(&window, 1000, 800, "Test Game"))
    {
        while (true)
        {
            fireWindowEvents(window);
        }
        destroyWindow(window);
    }

    return 0;
}
