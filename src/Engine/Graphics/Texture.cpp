#include "Texture.h"
#include "Utility/config.h"

namespace isaacObjectViewer
{    
    Texture::Texture()
        : 
        m_Width(0)
        , m_Height(0)
        , m_InternalFormat(GL_RGB)
        , m_ImageFormat(GL_RGB)
        , m_Wrap_S(GL_REPEAT)
        , m_Wrap_T(GL_REPEAT)
        , m_Filter_Min(GL_LINEAR)
        , m_Filter_Max(GL_LINEAR)
    {
        glGenTextures(1, &this->m_ID);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &m_ID);
    }
    void Texture::Generate(unsigned int width, unsigned int height, unsigned char* data)
    {
        this->m_Width = width;
        this->m_Height = height;
        // create Texture
        glBindTexture(GL_TEXTURE_2D, this->m_ID);
        glTexImage2D(GL_TEXTURE_2D, 0, this->m_InternalFormat, width, height, 0, this->m_ImageFormat, GL_UNSIGNED_BYTE, data);
        // set Texture wrap and filter modes
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->m_Wrap_S);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->m_Wrap_T);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->m_Filter_Min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->m_Filter_Max);
        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, this->m_ID);
    }
}