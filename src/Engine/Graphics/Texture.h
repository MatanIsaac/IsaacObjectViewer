#pragma once
#include "Utility/config.h"

namespace isaacObjectViewer
{
    class Texture
    {
    public:
        Texture();    
        ~Texture();
        // generates texture from image data
        void Generate(unsigned int width, unsigned int height, unsigned char* data, GLint internalFormat, GLenum dataFormat);
        
        // binds the texture as the current active GL_TEXTURE_2D texture object
        void Bind() const;

    private:
        // holds the ID of the texture object, used for all texture operations to reference to this particular texture
        unsigned int m_ID;
        // texture image dimensions
        unsigned int m_Width, m_Height; // width and height of loaded image in pixels
        // texture configuration
        unsigned int m_Wrap_S; // wrapping mode on S axis
        unsigned int m_Wrap_T; // wrapping mode on T axis
        unsigned int m_Filter_Min; // filtering mode if texture pixels < screen pixels
        unsigned int m_Filter_Max; // filtering mode if texture pixels > screen pixels
        // constructor (sets default texture modes)
        
    };
} // namespace isaacGraphicsEngine
