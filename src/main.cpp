#include "Engine/Core/Engine.h"

using namespace isaacGraphicsEngine;

int main()
{
    auto engine = Engine::GetInstance();
    if (engine)
    {
        engine->Run();
    }

    return 0;
}

