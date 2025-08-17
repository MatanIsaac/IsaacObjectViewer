/**
 * @brief Manages the loading and unloading of textures.
 * This class provides methods to load textures from files, unload them,
 * and keep track of all loaded textures.
 */

#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "Texture.h"
#include "Material.h"
#include "Utility/config.h"

namespace isaacObjectViewer
{
    class TextureManager 
    {
    public:

        /// @brief Loads a texture from a file.
        /// @param path The file path of the texture.
        /// @param type The type of the texture.
        /// @return A shared pointer to the loaded texture.
        static std::shared_ptr<Texture> LoadTexture(const std::string& path,TextureType type);

        /// @brief Unloads a texture.
        /// @param path The file path of the texture to unload.
        static void UnloadTexture(const std::string& path);

        /// @brief Unloads all textures.
        static void UnloadAll();

        /// @brief Gets the default material for the object.
        /// @return The default material for the object.
        static Material GetDefaultMaterial();
        

    private:
        /// @brief Constructs a TextureManager object.
        TextureManager() { }
        static std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
        static std::shared_ptr<Texture> LoadTextureFromFile(const std::string& path,TextureType type);

    };
    
} // namespace isaacGraphicsEngine
