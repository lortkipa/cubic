#include "core/logger.h"
#include <stdio.h>

const char* verbosityHeaders[] =
{ "ERROR", "WARNING", "SUCCESS", "INFO" };

const char* verbosityColors[] =
{ "\e[0;31m", "\e[0;33m", "\e[0;32m", "\e[0;34m" };

void LogV(const LogVerbosity verbosity, const char* channel, const char* format, va_list list)
{
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
