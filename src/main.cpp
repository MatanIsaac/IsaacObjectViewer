#include "Engine/Core/Engine.h"
#include "Utility/Log.hpp"

using namespace isaacGraphicsEngine;

int main()
{
    Log::Init();

    LOG_INFO("Welcome to Isaac's Graphics Engine!");

    auto engine = Engine::GetInstance();
    if (engine)
    {
        engine->Run();
    }

    return 0;
}
