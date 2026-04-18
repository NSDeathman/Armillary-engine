#include <iostream>
#include <Engine.h>
#include <Logger.h>

using namespace Armillary;

int main()
{
    Engine engine;

    bool bEngineInitialized = engine.Initialize();

    if(!bEngineInitialized)
    {
        LOG_ERROR("Failed to initialize engine");
        return -1;
    }

    engine.Run();
    engine.Shutdown();

    return 0;
}
