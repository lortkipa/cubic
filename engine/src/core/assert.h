#pragma once

#include "defines.h"

void logAssertionFailure(const char* channel, const char* resultStr, const char* message, const char* file, const char* function, const u32 line);

#if defined(DEBUG)
    
#define assert(channel, result, message)                    \
    if (!(result))                                          \
    {                                                       \
        logAssertionFailure(channel, #result, message,      \
                __FILE__, __FUNCTION__, __LINE__);          \
        __builtin_trap();                                   \
    }

#else

#define assert(channel, result, message)

#endif
