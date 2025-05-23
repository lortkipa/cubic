#pragma once

#include "defines.h"

#include <stdlib.h>

void outputAssertFailure(const char* const action, const char* const channel, const char* const message, const char* const file, const u32 line);

#define assert(channel, response, message)                                      \
    if (!(response))                                                            \
    {                                                                           \
        outputAssertFailure(#response, channel, message, __FILE__, __LINE__);   \
        exit(1);                                                                \
    }
