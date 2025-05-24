#include "core/debug/logger.h"

#include <stdarg.h>

const char* const color_strings[] =
{ "\e[0;34m", "\e[0;32m", "\e[0;33m", "\e[0;31m" }; 

const char* const header_strings[] =
{ "[INFO] ", "[SUCCESS] ", "[WARNING] ", "[ERROR] " }; 

void debugLogV(const log_severity severity, const char* const channel, const char* const format, va_list list)
{
    // set color and log level (depending on severity) + log channel
    printf("%s%s%s: ", color_strings[severity], header_strings[severity], channel);

    // log formated
    vprintf(format, list);

    // reset color and start new line
    puts("\e[0;37m");
}

void debugLog(const log_severity severity, const char* const channel, const char* const format, ...)
{
    // init args list
    va_list list;
    va_start(list, format);

    // log to console
    debugLogV(severity, channel, format, list);

    // terminate args list
    va_end(list);
}
