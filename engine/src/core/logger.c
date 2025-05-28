#include "core/logger.h"
#include <stdio.h>

const char* const color_strings[] =
{ "\e[0;34m", "\e[0;32m", "\e[0;33m", "\e[0;31m" };

const char* const header_strings[] =
{ "[INFO]", "[SUCCESS]", "[WARNING]", "[ERROR]" };

void debugLogV(const LogLevel level, const char* channel, const char* format, va_list list)
{
    // set color, header and channel
    printf("%s%s %s: ", color_strings[level], header_strings[level], channel);

    // log formated
    vprintf(format, list);

    // reset color and start new line
    puts("\e[0;37m");
}

void debugLog(const LogLevel level, const char* channel, const char* format, ...)
{
    // init list
    va_list list;
    va_start(list, format);

    // log formated with created list
    debugLogV(level, channel, format, list);

    // terminate list
    va_end(list);
}
