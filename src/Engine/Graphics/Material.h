#pragma once
#include <memory>

namespace isaacObjectViewer
{    
    class Texture;           

    struct Material
    {
        std::shared_ptr<Texture> Diffuse   { }; 
        std::shared_ptr<Texture> Specular  { };
        float                    Shininess { 32.0f };

        Material() = default;

        Material(std::shared_ptr<Texture> diffuse,
                std::shared_ptr<Texture> specular  = nullptr,
                float                       shininess = 32.0f)
            : Diffuse (std::move(diffuse))
            , Specular(std::move(specular))
            , Shininess(shininess)
        {}

        ~Material() = default;
    };
}