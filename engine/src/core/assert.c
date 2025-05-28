#include "core/assert.h"
#include "core/logger.h"

void logAssertionFailure(const char* channel, const char* resultStr, const char* message, const char* file, const char* function, const u32 line)
{
    // log formater error
    debugLogError(channel, "assertion failure: \"%s\": %s in %s %s() at line %d",
            resultStr, message, file, function, line);
}
