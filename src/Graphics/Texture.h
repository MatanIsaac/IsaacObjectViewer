#pragma once

#include "Utility/config.h"
namespace isaacObjectLoader
{
class Texture
{
public:
    Texture(const char* filename);
    ~Texture();
    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    int GetTextureWidth() const { return m_Width; }
    int GetTextureHeight() const { return m_Height; }

private:
    unsigned int   m_Texture;
    unsigned char* m_LocalBuffer;
    int            m_Width, m_Height, m_BPP;
};
}  // namespace isaacObjectLoader