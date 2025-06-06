#pragma once

#include "defines.h"

void LogAssertionFailure(const char* channel, const char* message, const char* errorString, const char* file, const char* function, const u32 line);

#if defined(DEBUG)

#define assert(channel, result, message)                                                     \
    if (!(result))                                                                           \
    {                                                                                        \
        LogAssertionFailure(channel, message, #result, __FILE__, __FUNCTION__, __LINE__);    \
        __builtin_trap();                                                                    \
    }

#elif 

#define assert(channel, result, message)

#endif
