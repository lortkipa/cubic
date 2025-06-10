#pragma once

#include "defines.h"
#include <stdarg.h>

#define LOG_VERBOSITY_FLAG_ERROR 0x01
#define LOG_VERBOSITY_FLAG_WARNING 0x02
#define LOG_VERBOSITY_FLAG_SUCCESS 0x04
#define LOG_VERBOSITY_FLAG_INFO 0x08

typedef enum LogVerbosity {
    LOG_VERBOSITY_ERROR,
    LOG_VERBOSITY_WARNING,
    LOG_VERBOSITY_SUCCESS,
    LOG_VERBOSITY_INFO
} LogVerbosity;

EXPORT b8 StartupLogSystem(const u8 flags);

EXPORT void ShutdownLogSystem(void);

u8 GetLogVerbosityFlag(const LogVerbosity verbosity);

void LogV(const LogVerbosity verbosity, const char* channel, const char* format, va_list list);

void Log(const LogVerbosity verbosity, const char* channel, const char* format, ...);

#define LogError(channel, format, ...) Log(LOG_VERBOSITY_ERROR, channel, format, ##__VA_ARGS__)

#define LogWarning(channel, format, ...) Log(LOG_VERBOSITY_WARNING, channel, format, ##__VA_ARGS__)

#define LogSuccess(channel, format, ...) Log(LOG_VERBOSITY_SUCCESS, channel, format, ##__VA_ARGS__)

#define LogInfo(channel, format, ...) Log(LOG_VERBOSITY_INFO, channel, format, ##__VA_ARGS__)
