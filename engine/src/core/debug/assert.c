#include "core/debug/assert.h"

#include "core/debug/logger.h"

void outputAssertFailure(const char* const action, const char* const channel, const char* const message, const char* const file, const u32 line)
{
    debugLogError(channel,
            "assertion failure \"%s\": %s in %s at %d",
            action, message, file, line);
}
