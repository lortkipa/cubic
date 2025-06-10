#include "core/logger.h"
#include "defines.h"
#include <stdio.h>

#define CHANNEL "Logger"

static u8 verbosityFlags = 0;

static const char* verbosityHeaders[] =
{ "ERROR", "WARNING", "SUCCESS", "INFO" };

static const char* verbosityColors[] =
{ "\e[0;31m", "\e[0;33m", "\e[0;32m", "\e[0;34m" };

b8 StartupLogSystem(const u8 flags)
{
    // check if provided flags are in correct bound
    if (
            (flags & (
                      LOG_VERBOSITY_FLAG_ERROR |
                      LOG_VERBOSITY_FLAG_WARNING |
                      LOG_VERBOSITY_FLAG_SUCCESS |
                      LOG_VERBOSITY_FLAG_INFO
                     )) == 0)
    {
        return false;
    }

    // asign provided flags to verbosity flags
    verbosityFlags = flags;

    // return success as default
    LogSuccess(CHANNEL, SYSTEM_INITIALIZED_MESSAGE);
    return true;
}

u8 GetLogVerbosityFlag(const LogVerbosity verbosity)
{
    switch (verbosity) 
    {
        case LOG_VERBOSITY_ERROR:
            return LOG_VERBOSITY_FLAG_ERROR;
            break;
        case LOG_VERBOSITY_WARNING:
            return LOG_VERBOSITY_FLAG_WARNING;
            break;
        case LOG_VERBOSITY_SUCCESS:
            return LOG_VERBOSITY_FLAG_SUCCESS;
            break;
        case LOG_VERBOSITY_INFO:
            return LOG_VERBOSITY_FLAG_INFO;
            break;
        default:
            LogWarning(CHANNEL, "Invalid Verbosity Provided");
            return 0;
            break;
    }
}

void ShutdownLogSystem(void)
{
    // zero out verbosity flags
    LogSuccess(CHANNEL, SYSTEM_TERMINATED_MESSAGE);
    verbosityFlags = 0;
}

void LogV(const LogVerbosity verbosity, const char* channel, const char* format, va_list list)
{
    // make sure current verbosity is enabled
    if (!(verbosityFlags & GetLogVerbosityFlag(verbosity)))
    {
        return;
    }

    // set color and log header and channel
    printf("%s[%s] %s: ", verbosityColors[verbosity], verbosityHeaders[verbosity], channel);

    // log body of log
    vprintf(format, list);

    // reset color and start new line
    puts("\e[0;37m");
}

void Log(const LogVerbosity verbosity, const char* channel, const char* format, ...)
{
    // init list
    va_list list;
    va_start(list, format);

    // log formated
    LogV(verbosity, channel, format, list);

    // terminate list
    va_end(list);
}
