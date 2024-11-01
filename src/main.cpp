#include "Core/Engine.h"

using namespace isaacObjectLoader;

int main()
{
    std::cout << "Welcome to Isaac's Object Loader!\n";
    auto engine = Engine::get();
    if (engine)
    {
        engine->RunEngine();
    }
    return 0;
}