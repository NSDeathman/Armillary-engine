#include <iostream>
#include <Engine.h>
#include <Logger.h>
#include <AfterMath\AfterMath.h>

using namespace Armillary;

void TestAfterMath()
{
    float3 a(1.0f, 2.0f, 3.0f);
    float3 b(4.0f, 5.0f, 6.0f);
    float3 c = a + b;
    LOG_INFO("Vector addition: (" + std::to_string(c.x) + ", " + std::to_string(c.y) + ", " + std::to_string(c.z) + ")");
}

int main()
{
    Engine engine;

    bool bEngineInitialized = engine.Initialize();

    if(!bEngineInitialized)
    {
        LOG_ERROR("Failed to initialize engine");
        return -1;
    }

    TestAfterMath();

    engine.Run();
    engine.Shutdown();

    return 0;
}
