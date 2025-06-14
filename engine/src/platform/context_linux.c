#include "platform/context.h"

#if defined(PLATFORM_LINUX)

#define CHANNEL "GLX Context Manager"

#include "platform/window_linux.h"
#include "core/logger.h"
#include <GL/glx.h>

typedef GLXContext (*glXCreateContextAttribsARBProc)
    (Display*, GLXFBConfig, GLXContext, Bool, const int*);

static GLXContext context;

b8 CreateGLContext(const u32 majorV, const u32 minorV)
{
    // load gl functions
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB =
        (glXCreateContextAttribsARBProc)
        glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

    // check if function is loaded
    if (!glXCreateContextAttribsARB)
    {
        LogError(CHANNEL, "glXCreateContextAttribsARB() Not Loaded");
        return false;
    }

    // context attributes
    int visual_attribs[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER, true,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        None
    };

    // get framebuffer configs
    i32 frameBufferConfigCount;
    GLXFBConfig* frameBufferConfigs = glXChooseFBConfig
        (
            GetLinuxWindowDisplay(),
            GetLinuxWindowScreen(),
            visual_attribs,
            &frameBufferConfigCount
            );

    // check if frame buffer configs are found
    if (!frameBufferConfigs)
    {
        LogError(CHANNEL, "Frame Buffer Config Not Found");
        return false;
    }
    LogInfo(CHANNEL, "%d Frame Buffer Config Found", frameBufferConfigCount);

    // choose first frame buffer config
    GLXFBConfig frameBufferConfig = frameBufferConfigs[0];

    // free frame buffer configs array
    XFree(frameBufferConfigs);

    // opengl version
    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, majorV,
        GLX_CONTEXT_MINOR_VERSION_ARB, minorV,
        GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };
    
    // create context
    context = glXCreateContextAttribsARB
        (
         GetLinuxWindowDisplay(),
         frameBufferConfig,
         null,
         true,
         context_attribs
         );

    // check for errors
    if (!context)
    {
        LogError(CHANNEL, "Failed To Create %d.%d Context", majorV, minorV);
    }
    LogSuccess(CHANNEL, "%d.%d Context Created", majorV, minorV);

    // if code comes here, everythings good, so return success
    return true;
}

void BindGLContext(void)
{
    // make context current
    glXMakeCurrent(GetLinuxWindowDisplay(), GetLinuxWindowHandle(), context);
}

void SwapGLBuffers(void)
{
    // swap buffers
    glXSwapBuffers(GetLinuxWindowDisplay(), GetLinuxWindowHandle());
}

#endif
