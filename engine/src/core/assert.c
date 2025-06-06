#include "core/assert.h"
#include "core/logger.h"

void LogAssertionFailure(const char* channel, const char* message, const char* errorString, const char* file, const char* function, const u32 line)
{
    LogError(channel, 
            "Assertion Failure \"%s\": %s { file: %s, function: %s, line: %d }",
            errorString, message, file, function, line);
}
