#pragma once

#include "defines.h"

#define DebugBreak() __builtin_trap();

void LogAssertionFailure(const char* channel, const char* error,
        const char* message, const char* file, const char* function, const u32 line);

#define Assert(channel, result, message)                                                    \
    if (!(result))                                                                          \
    {                                                                                       \
        LogAssertionFailure(channel, #result, message, __FILE__, __FUNCTION__, __LINE__);   \
        DebugBreak();                                                                       \
    }
