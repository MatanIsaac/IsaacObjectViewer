#include "Texture.h"

namespace isaacObjectViewer
{    
    Texture::Texture()
        : m_Type(TextureType::NONE)
        , m_Width(0)
        , m_Height(0)
        , m_Wrap_S(GL_REPEAT)
        , m_Wrap_T(GL_REPEAT)
        , m_Filter_Min(GL_LINEAR_MIPMAP_LINEAR)
        , m_Filter_Max(GL_LINEAR)
    {
        glGenTextures(1, &this->m_ID);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &m_ID);
    }
    void Texture::Generate(unsigned int width, unsigned int height, unsigned char* data, GLint internalFormat, GLenum dataFormat,TextureType type)
    {
        m_Type = type;
        this->m_Width = width;
        this->m_Height = height;
        
        // create Texture
        glBindTexture(GL_TEXTURE_2D, this->m_ID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // set Texture wrap and filter modes
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->m_Wrap_S);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->m_Wrap_T);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->m_Filter_Min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->m_Filter_Max);
        
        if (internalFormat == GL_RED)                 
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
        }
        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

    }

    void Texture::Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, this->m_ID);
    }
}