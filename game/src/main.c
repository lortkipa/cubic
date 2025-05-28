#include "core/logger.h"
#include "core/assert.h"

int main(void)
{
    debugLogInfo("Game", "Logging Info %f", 3.14f);
    debugLogSuccess("Game", "Logging Success %f", 3.14f);
    debugLogWarning("Game", "Logging Warning %f", 3.14f);
    debugLogError("Game", "Logging Error %f", 3.14f);

    assert("Game", 1 == 1, "shit! app failed!");
    assert("Game", 1 == 2, "shit! app failed!");

    return 0;
}
