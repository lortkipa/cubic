#include <core/logger.h>
#include <core/assert.h>

int main(void)
{
    debugLogInfo("logger", "logging info: %f", 3.14f);
    debugLogSuccess("logger", "logging success: %f", 3.14f);
    debugLogWarning("logger", "logging warning: %f", 3.14f);
    debugLogError("logger", "logging error: %f", 3.14f);
    assert("asserter", 4 == 6, "shit! app failed!");
}
