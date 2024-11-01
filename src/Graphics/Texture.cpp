#include "Texture.h"
#include "../third_party/stb_image.h"
#include <cassert>

namespace isaacObjectLoader
{
Texture::Texture(const char* filename)
: m_LocalBuffer(nullptr)
, m_Width(0)
, m_Height(0)
, m_BPP(0)
{
    // create the texture
    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image
    stbi_set_flip_vertically_on_load(true);
    m_LocalBuffer = stbi_load(filename, &m_Width, &m_Height, &m_BPP, STBI_rgb_alpha);

    assert(m_LocalBuffer != nullptr);

    if (m_LocalBuffer)
    {
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     m_Width,
                     m_Height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     m_LocalBuffer);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(m_LocalBuffer);
}

Texture::~Texture() { glDeleteTextures(1, &m_Texture); }

void Texture::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
}

void Texture::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
}  // namespace isaacObjectLoader