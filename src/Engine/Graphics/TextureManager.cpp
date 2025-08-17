#include "TextureManager.h"
#include "stb_image.h"
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{
    std::unordered_map<std::string, std::shared_ptr<Texture>> TextureManager::m_Textures;
    
    std::shared_ptr<Texture> TextureManager::LoadTexture(const std::string &path, TextureType type)
    {
        m_Textures[path] = LoadTextureFromFile(path,type);
        return m_Textures[path];
    }

    std::shared_ptr<Texture> TextureManager::LoadTextureFromFile(const std::string &path, TextureType type)
    {
        // check if already exists exists
        auto it = m_Textures.find(path);
        if (it != m_Textures.end()) 
            return it->second;

        // create texture object
        auto texture = std::make_shared<Texture>();
        texture->SetType(type);
        texture->SetPath(path);
        // load image
        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if(data)
        {
            GLenum internalFormat = GL_RGB;
            GLenum dataFormat = GL_RGB;
            if (nrChannels == 1) 
            {
                internalFormat = GL_RED;
                dataFormat = GL_RED;
            } 
            else if (nrChannels == 3) 
            {
                internalFormat = GL_RGB;
                dataFormat = GL_RGB;
            } 
            else if (nrChannels == 4) 
            {
                internalFormat = GL_RGBA;
                dataFormat = GL_RGBA;
            }

            
            texture->Generate(width, height, data, internalFormat,dataFormat,type);
        }
        else 
        {
            LOG_ERROR("Failed to load texture at: {}",path);
            return nullptr;
        }
        
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
    Material TextureManager::GetDefaultMaterial()
    {
        std::string diffuseMap = GetProjectRootPath("/src/Resources/Textures/stonewall-01/stonewall-01_albedo.png");
        std::string normalMap = GetProjectRootPath("/src/Resources/Textures/stonewall-01/stonewall-01_normal.png");
        std::string specularMap = GetProjectRootPath("/src/Resources/Textures/stonewall-01/stonewall-01_roughness.png");

        auto diffuse = TextureManager::LoadTexture(diffuseMap,TextureType::DIFFUSE);
        auto normal = TextureManager::LoadTexture(normalMap,TextureType::NORMAL);
        auto specular = TextureManager::LoadTexture(specularMap,TextureType::SPECULAR);

        return Material(std::shared_ptr<Texture>(diffuse),std::shared_ptr<Texture>(normal),std::shared_ptr<Texture>(specular),32.f);
    }
}