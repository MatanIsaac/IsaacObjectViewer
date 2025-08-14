/**
 * @brief Represents a texture in the engine.
 * This class encapsulates the properties and behaviors of a texture,
 * including its ID, path, type, and various texture settings.
 */

#pragma once
#include "Utility/config.h"

namespace isaacObjectViewer
{
    /// @brief Represents the type of a texture.
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
        /// @brief Constructs a Texture object.
        Texture();    

        /// @brief Destroys the Texture object.
        ~Texture();

        /// @brief Generates a texture from image data.
        void Generate(unsigned int width, unsigned int height, unsigned char* data, GLint internalFormat, GLenum dataFormat, TextureType type);

        /// @brief Binds the texture as the current active GL_TEXTURE_2D texture object.
        void Bind() const;

        /// @brief Gets the ID of the texture.
        /// @return The ID of the texture.
        unsigned int GetID() const { return m_ID; }

        /// @brief Gets the type of the texture.
        /// @return The type of the texture.
        TextureType GetType() const { return m_Type; }

        /// @brief Sets the type of the texture.
        /// @param type The new type of the texture.
        void SetType(TextureType type) { m_Type = type; }

        /// @brief Gets the path of the texture.
        /// @return The path of the texture.
        const std::string& GetPath() const { return m_Path; }

        /// @brief Sets the path of the texture.
        /// @param newPath The new path of the texture.
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
