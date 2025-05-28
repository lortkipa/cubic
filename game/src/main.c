#include "core/logger.h"

int main(void)
{
    debugLogInfo("Game", "Logging Info %f", 3.14f);
    debugLogSuccess("Game", "Logging Success %f", 3.14f);
    debugLogWarning("Game", "Logging Warning %f", 3.14f);
    debugLogError("Game", "Logging Error %f", 3.14f);

    return 0;
}
