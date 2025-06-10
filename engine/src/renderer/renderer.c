#include "renderer/renderer.h"
#include "core/assert.h"
#include "core/logger.h"

#define CHANNEL "Renderer"

static b8 initialized = false;

b8 StartupEngine(void)
{
    // make sure system is not started
    Assert(CHANNEL, initialized == false, "System Is Already Initialized");

    // track startup
    initialized = true;
    LogSuccess(CHANNEL, SYSTEM_INITIALIZED_MESSAGE);

    // return success
    return true;
}

void ShutdownEngine(void)
{
    // make sure system is started
    Assert(CHANNEL, initialized == true, "System Is Not Initialized Yet");

    // track shutdown
    initialized = false;
    LogSuccess(CHANNEL, SYSTEM_TERMINATED_MESSAGE);
}
