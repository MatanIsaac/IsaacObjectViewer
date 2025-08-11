#pragma once
#include "Utility/config.h"

namespace isaacObjectViewer
{
    enum class TextureType
    {
        NONE = 0,
        DIFFUSE,
        SPECULAR,
        NORMAL,
        HEIGHT,
        TOTAL
    };

    class Texture
    {
    public:
        Texture();    
        ~Texture();
        // generates texture from image data
        void Generate(unsigned int width, unsigned int height, unsigned char* data, GLint internalFormat, GLenum dataFormat,TextureType type);

        // binds the texture as the current active GL_TEXTURE_2D texture object
        void Bind() const;

        unsigned int GetID() const { return m_ID; }
        TextureType GetType() const { return m_Type; }

        void SetType(TextureType type) { m_Type = type; }

        const std::string& GetPath() const { return m_Path; }
        void SetPath(const std::string& newPath) { m_Path = newPath; }

    private:
        // holds the ID of the texture object, used for all texture operations to reference to this particular texture
        unsigned int m_ID;
        std::string m_Path; // path to the texture file
        TextureType m_Type;
        
        // texture image dimensions
        unsigned int m_Width, m_Height; // width and height of loaded image in pixels
        // texture configuration
        unsigned int m_Wrap_S; // wrapping mode on S axis
        unsigned int m_Wrap_T; // wrapping mode on T axis
        unsigned int m_Filter_Min; // filtering mode if texture pixels < screen pixels
        unsigned int m_Filter_Max; // filtering mode if texture pixels > screen pixels
    };
} // namespace isaacGraphicsEngine
