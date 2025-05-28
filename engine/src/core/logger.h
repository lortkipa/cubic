#pragma once

#include "defines.h"
#include <stdarg.h>

typedef enum LogLevel {
    LOG_LEVEL_INFO,
    LOG_LEVEL_SUCCESS,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
} LogLevel;

void debugLogV(const LogLevel level, const char* channel, const char* format, va_list list);

void debugLog(const LogLevel level, const char* channel, const char* format, ...);

#if defined(DEBUG)

#define debugLogInfo(channel, format, ...) debugLog(LOG_LEVEL_INFO, channel, format, ##__VA_ARGS__)
#define debugLogSuccess(channel, format, ...) debugLog(LOG_LEVEL_SUCCESS, channel, format, ##__VA_ARGS__)

#else

#define debugLogInfo(channel, format, ...)
#define debugLogSuccess(channel, format, ...)

#endif

#define debugLogWarning(channel, format, ...) debugLog(LOG_LEVEL_WARNING, channel, format, ##__VA_ARGS__)
#define debugLogError(channel, format, ...) debugLog(LOG_LEVEL_ERROR, channel, format, ##__VA_ARGS__)
