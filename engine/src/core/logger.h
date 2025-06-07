#pragma once

#include "defines.h"
#include <stdarg.h>
#include <string.h>

#define LOG_VERBOSITY_FLAG_ERROR 0x1
#define LOG_VERBOSITY_FLAG_WARNING 0x2
#define LOG_VERBOSITY_FLAG_SUCCESS 0x4
#define LOG_VERBOSITY_FLAG_INFO 0x8
#define LOG_VERBOSITY_FLAG_EVERY LOG_VERBOSITY_FLAG_ERROR | LOG_VERBOSITY_FLAG_WARNING | LOG_VERBOSITY_FLAG_SUCCESS | LOG_VERBOSITY_FLAG_INFO

typedef enum LogVerbosity {
    LOG_VERBOSITY_ERROR,
    LOG_VERBOSITY_WARNING,
    LOG_VERBOSITY_SUCCESS,
    LOG_VERBOSITY_INFO
} LogVerbosity;

u8 GetLogVerbosityFlag(const LogVerbosity verbosity);

bool StartupLogSystem(const u8 flags);

void ShutdownLogSystem(void);

void LogV(const LogVerbosity verbosity, const char* channel, const char* format, va_list list);

void Log(const LogVerbosity verbosity, const char* channel, const char* format, ...);

#define LogError(channel, format, ...) Log(LOG_VERBOSITY_ERROR, channel, format, ##__VA_ARGS__)

#define LogWarning(channel, format, ...) Log(LOG_VERBOSITY_WARNING, channel, format, ##__VA_ARGS__)

#define LogSuccess(channel, format, ...) Log(LOG_VERBOSITY_SUCCESS, channel, format, ##__VA_ARGS__)

#define LogInfo(channel, format, ...) Log(LOG_VERBOSITY_INFO, channel, format, ##__VA_ARGS__)
