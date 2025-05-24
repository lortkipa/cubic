#pragma once

#include "defines.h"

#include <stdio.h>

typedef enum log_severity {
    LOG_SEVERITY_INFO,
    LOG_SEVERITY_SUCCESS,
    LOG_SEVERITY_WARNING,
    LOG_SEVERITY_ERROR
} log_severity;

void debugLogV(const log_severity severity, const char* const channel, const char* const format, va_list list);

void debugLog(const log_severity severity, const char* const channel, const char* const format, ...);

#define debugLogInfo(channel, format, ...) debugLog(LOG_SEVERITY_INFO, channel, format, ##__VA_ARGS__)

#define debugLogSuccess(channel, format, ...) debugLog(LOG_SEVERITY_SUCCESS, channel, format, ##__VA_ARGS__)

#define debugLogWarning(channel, format, ...) debugLog(LOG_SEVERITY_WARNING, channel, format, ##__VA_ARGS__)

#define debugLogError(channel, format, ...) debugLog(LOG_SEVERITY_ERROR, channel, format, ##__VA_ARGS__)
