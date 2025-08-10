#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "Texture.h"

namespace isaacObjectViewer
{
    class TextureManager 
    {
    public:
        static std::shared_ptr<Texture> LoadTexture(const std::string& path,TextureType type);
        static void UnloadTexture(const std::string& path);
        static void UnloadAll();

    private:
        TextureManager() { }
        static std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
        static std::shared_ptr<Texture> LoadTextureFromFile(const std::string& path,TextureType type);

    };

} // namespace isaacGraphicsEngine
