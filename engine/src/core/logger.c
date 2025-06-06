#include "core/logger.h"
#include <stdio.h>

// verbosity flags
static u8 verbosityFlags = 0;

// verbosity strings
static const char* header_strings[] = 
{ "ERROR", "WARNING", "SUCCESS", "INFO" };

// color strings depending on verbosity
static const char* color_strings[] = 
{ "\e[0;31m", "\e[0;33m", "\e[0;32m", "\e[0;34m" };

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
    }
}

bool StartupLogSystem(const u8 flags)
{
    // check if any flags are provided
    if (
            !(flags & LOG_VERBOSITY_FLAG_ERROR) &&
            !(flags & LOG_VERBOSITY_FLAG_WARNING) &&
            !(flags & LOG_VERBOSITY_FLAG_SUCCESS) &&
            !(flags & LOG_VERBOSITY_FLAG_INFO)
       )
    {
        return false;
    }

    // store provided flags
    verbosityFlags = flags;

    // return success as default
    LogSuccess("Log System", "system initialized");
    return true;
}

void ShutdownLogSystem(void)
{
    // remove any verbosity flag
    LogSuccess("Log System", "system terminated");
    verbosityFlags = 0;
}

void LogV(const LogVerbosity verbosity, const char* channel, const char* format, va_list list)
{
    // check if this verbosity should be logged or not
    if (!(verbosityFlags & GetLogVerbosityFlag(verbosity)))
    {
        return;
    }

    // set color and log header and channel
    printf("%s[%s] %s: ", color_strings[verbosity], header_strings[verbosity], channel);

    // log formated
    vprintf(format, list);

    // start new line
    puts("\e[0;37m");
}

void Log(const LogVerbosity verbosity, const char* channel, const char* format, ...)
{
    // check if this verbosity should be logged or not
    if (!(verbosityFlags & GetLogVerbosityFlag(verbosity)))
    {
        return;
    }

    // init va_list
    va_list list;
    va_start(list, format);

    // log
    LogV(verbosity, channel, format, list);

    // terminate va_list
    va_end(list);
}
