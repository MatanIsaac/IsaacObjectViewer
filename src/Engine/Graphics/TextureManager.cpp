#include "TextureManager.h"
#include "stb_image.h"
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{
    std::unordered_map<std::string, std::shared_ptr<Texture>> TextureManager::m_Textures;
    
    static GLenum chooseInternalFormat(int channels, bool srgb) 
    {
        switch (channels) 
        {
            case 1: return GL_R8;
            case 3: return srgb ? GL_SRGB8 : GL_RGB8;
            case 4: return srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
            default: return GL_RGB8;
        }
    }
    static GLenum chooseDataFormat(int channels) 
    {
        switch (channels) 
        {
            case 1: return GL_RED;
            case 3: return GL_RGB;
            case 4: return GL_RGBA;
            default: return GL_RGB;
        }
    }
    std::string TextureManager::makeKey(const std::string& id, TextureType type, bool srgb) 
    {
        std::ostringstream oss;
        oss << id << '|' << static_cast<int>(type) << '|' << (srgb ? 'S' : 'L');
        return oss.str();
    }

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
        return texture;
    }

    std::shared_ptr<Texture> TextureManager::LoadTextureFromMemory(const unsigned char *bytes, size_t size, TextureType type, bool srgb, const char *debugName)
    {
        const auto key = makeKey(std::string("mem:") + (debugName ? debugName : ""), type, srgb);
        if (auto it = m_Textures.find(key); it != m_Textures.end())
            return it->second;

        auto tex = loadFromBytesInternal(bytes, size, type, srgb, debugName);
        if (tex) 
        {
            LOG_INFO("Loaded texture from mem");
            m_Textures.emplace(key, tex);
        }
        return tex;
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

    std::shared_ptr<Texture> TextureManager::loadFromFileInternal(const std::string& path,
                                                              TextureType type, bool srgb) 
    {
        int w = 0, h = 0, ch = 0;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 0);
        if (!data) 
        {
            LOG_ERROR("Failed to load texture '{}': {}", path, stbi_failure_reason());
            return nullptr;
        }

        auto tex = std::make_shared<Texture>();
        tex->SetType(type);
        tex->SetPath(path);

        const GLenum internal = chooseInternalFormat(ch, srgb);
        const GLenum format   = chooseDataFormat(ch);

        tex->Generate(w, h, data, internal, format, type);
        stbi_image_free(data);
        return tex;
    }

    std::shared_ptr<Texture> TextureManager::loadFromBytesInternal(const unsigned char* bytes,
                                                                size_t size,
                                                                TextureType type, bool srgb,
                                                                const char* debugName) 
    {
        int w = 0, h = 0, ch = 0;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load_from_memory(bytes, (int)size, &w, &h, &ch, 0);
        if (!data) 
        {
            LOG_ERROR("Failed to load embedded texture '{}': {}", debugName ? debugName : "?",
                    stbi_failure_reason());
            return nullptr;
        }

        auto tex = std::make_shared<Texture>();
        tex->SetType(type);
        if (debugName) 
            tex->SetPath(std::string("embedded:") + debugName);

        const GLenum internal = chooseInternalFormat(ch, srgb);
        const GLenum format   = chooseDataFormat(ch);

        tex->Generate(w, h, data, internal, format, type);
        stbi_image_free(data);
        return tex;
    }

}
