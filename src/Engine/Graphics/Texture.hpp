#pragma once
#include <string>

namespace isaacGraphicsEngine
{
    struct Texture
    {
        // The id of the texture
        unsigned int id;
        // The type e.g diffuse or specular texture
        std::string type;
        // The path of the texture
        std::string path;
    };
} // namespace isaacGraphicsEngine
