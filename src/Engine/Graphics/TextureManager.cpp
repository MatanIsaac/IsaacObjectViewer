#include "TextureManager.h"
#include "stb_image.h"
#include "Utility/config.h"

namespace isaacObjectViewer
{
    std::unordered_map<std::string, std::shared_ptr<Texture>> TextureManager::m_Textures;
    
    std::shared_ptr<Texture> TextureManager::LoadTexture(const std::string &path, bool alpha)
    {
        m_Textures[path] = LoadTextureFromFile(path,alpha);
        return m_Textures[path];
    }

    std::shared_ptr<Texture> TextureManager::LoadTextureFromFile(const std::string &path, bool alpha)
    {
        // create texture object
        auto it = m_Textures.find(path);
        if (it != m_Textures.end()) 
            return it->second;
        auto texture = std::make_shared<Texture>();
        if (alpha)
        {
            texture->SetInternalFormat(GL_RGBA);
            texture->SetImageFormat(GL_RGBA);
        }
        // load image
        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        // now generate texture
        texture->Generate(width, height, data);
        // and finally free image data
        stbi_image_free(data);
        m_Textures[path] = texture;
        return texture;
    }
    
    void TextureManager::UnloadTexture(const std::string &path)
    {
        m_Textures.erase(path);
    }

    void TextureManager::UnloadAll()
    {
        m_Textures.clear();
    }
}