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

        /// @brief Loads a texture from memory.
        /// @param bytes The raw image data.
        /// @param size The size of the image data.
        /// @param type The type of the texture.
        /// @param srgb Whether the texture is in sRGB color space.
        /// @param debugName An optional debug name for the texture.
        /// @return A shared pointer to the loaded texture.
        static std::shared_ptr<Texture> LoadTextureFromMemory(const unsigned char* bytes,
                                                          size_t size,
                                                          TextureType type,
                                                          bool srgb = false,
                                                          const char* debugName = nullptr);

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

        /// @brief Loads a texture from a file.
        /// @param path The file path of the texture.
        /// @param type The type of the texture.
        /// @return A shared pointer to the loaded texture.
        static std::shared_ptr<Texture> LoadTextureFromFile(const std::string& path,TextureType type);

        /// @brief Creates a unique key for the texture.
        /// @param id The identifier of the texture.
        /// @param type The type of the texture.
        /// @param srgb Whether the texture is in sRGB color space.
        /// @return A unique key for the texture.
        static std::string makeKey(const std::string& id, TextureType type, bool srgb);

        /// @brief Loads a texture from a file.
        /// @param path The file path of the texture.
        /// @param type The type of the texture.
        /// @param srgb Whether the texture is in sRGB color space.
        /// @return A shared pointer to the loaded texture.
        static std::shared_ptr<Texture> loadFromFileInternal(const std::string& path,
                                                            TextureType type, bool srgb);

        /// @brief Loads a texture from memory.
        /// @param bytes The raw image data.
        /// @param size The size of the image data.
        /// @param type The type of the texture.
        /// @param srgb Whether the texture is in sRGB color space.
        /// @param debugName An optional debug name for the texture.
        /// @return A shared pointer to the loaded texture.
        static std::shared_ptr<Texture> loadFromBytesInternal(const unsigned char* bytes,
                                                            size_t size,
                                                            TextureType type, bool srgb,
                                                            const char* debugName);

    };
    
} // namespace isaacGraphicsEngine
