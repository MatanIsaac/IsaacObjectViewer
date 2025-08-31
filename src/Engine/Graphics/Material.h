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

        /// @brief The normal texture of the material.
        std::shared_ptr<Texture> Normal    { };

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
                std::shared_ptr<Texture> normal    = nullptr,
                std::shared_ptr<Texture> specular  = nullptr,
                float                       shininess = 32.0f)
            : Diffuse (std::move(diffuse))
            , Normal  (std::move(normal))
            , Specular(std::move(specular))
            , Shininess(shininess)
        {}

        /// @brief Move constructor.
        Material(Material&& other) noexcept
            : Diffuse(std::move(other.Diffuse))
            , Normal(std::move(other.Normal))
            , Specular(std::move(other.Specular))
            , Shininess(other.Shininess)
        {}

        /// @brief Move assignment operator.
        Material& operator=(Material&& other) noexcept
        {
            if (this != &other)
            {
                Diffuse   = std::move(other.Diffuse);
                Normal    = std::move(other.Normal);
                Specular  = std::move(other.Specular);
                Shininess = other.Shininess;
            }
            return *this;
        }

        /// @brief Copy constructor.
        Material(const Material& other)
            : Diffuse(other.Diffuse)
            , Normal(other.Normal)
            , Specular(other.Specular)
            , Shininess(other.Shininess)
        {}

        /// @brief Copy assignment operator.
        Material& operator=(const Material& other)
        {
            if (this != &other)
            {
                Diffuse   = other.Diffuse;
                Normal    = other.Normal;
                Specular  = other.Specular;
                Shininess = other.Shininess;
            }
            return *this;
        }
        /// @brief Destructor.
        ~Material() = default;

    };
}