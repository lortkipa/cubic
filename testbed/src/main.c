#include "core/logger.h"

int main(void)
{
    LogError("Game", "Hello World %f", 3.14f);
    LogWarning("Game", "Hello World %f", 3.14f);
    LogSuccess("Game", "Hello World %f", 3.14f);
    LogInfo("Game", "Hello World %f", 3.14f);
}
