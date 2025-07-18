#include "Engine/Core/Engine.h"
#include <SDL3/SDL_main.h>

using namespace isaacObjectViewer;

int main(int argc, char* argv[])
{   
    auto engine = Engine::GetInstance();
    if (engine)
    {
        engine->Run();
    }

    return 0;
}

