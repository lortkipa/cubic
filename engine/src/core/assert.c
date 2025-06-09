#include "core/assert.h"
#include "core/logger.h"

void LogAssertionFailure(const char* channel, const char* error,
        const char* message, const char* file, const char* function, const u32 line)
{
    // log assertion failure with provided values
    LogError(channel,
            "Assertion Failure \"%s\": %s { File: \"%s\", Function: \"%s\", Line: %d }",
            error, message, file, function, line);
}
