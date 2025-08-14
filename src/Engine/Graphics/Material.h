/**
 * @file Material.h
 * @brief Header file for the Material struct.
 * This struct is responsible for defining the material properties used in the rendering pipeline.
 * Currently handles only diffuse and specular textures.
 */

#pragma once
#include <memory>

namespace isaacObjectViewer
{    
    class Texture;

    struct Material
    {
        /// @brief The diffuse texture of the material.
        std::shared_ptr<Texture> Diffuse   { }; 
        /// @brief The specular texture of the material.
        std::shared_ptr<Texture> Specular  { };
        /// @brief The shininess factor of the material.
        float                    Shininess { 32.0f };

        /// @brief Default constructor.
        Material() = default;

        /// @brief Parameterized constructor.
        /// @param diffuse The diffuse texture of the material.
        /// @param specular The specular texture of the material.
        /// @param shininess The shininess factor of the material.
        Material(std::shared_ptr<Texture> diffuse,
                std::shared_ptr<Texture> specular  = nullptr,
                float                       shininess = 32.0f)
            : Diffuse (std::move(diffuse))
            , Specular(std::move(specular))
            , Shininess(shininess)
        {}

        /// @brief Destructor.
        ~Material() = default;
    };
}