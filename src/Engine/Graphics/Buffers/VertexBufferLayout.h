/**
 *  @file VertexBufferLayout.h
 *  @brief Header file for the VertexBufferLayout class.
 *   This class represents the layout of a vertex buffer in the graphics pipeline.
 *   It manages the organization and attributes of vertex data for rendering.
 */

#pragma once

#include "Utility/config.h"
#include "Utility/GLErrorManager.h"

namespace isaacObjectViewer
{

struct VertexBufferElement
{
    unsigned int m_Count; // The number of components per vertex attribute
    unsigned int m_Type;  // The data type of the vertex attribute
    bool         m_Normalized; // Whether the attribute is normalized

    /// @brief  Gets the size of a specific type.
    /// @param type The type to get the size of.
    /// @return The size of the type in bytes.
    static unsigned int GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
            case GL_FLOAT:
                return 4;
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:
                return 1;
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout
{
public:
    /// @brief Constructs a VertexBufferLayout.
    VertexBufferLayout();

    /// @brief  Pushes a new element into the layout.
    /// @tparam T The type of the element.
    /// @param count The number of elements.
    template<typename T> 
    void Push(unsigned int count);

    /// @brief  Gets the stride of the layout.
    /// @return The stride in bytes.
    inline unsigned int GetStride() const { return m_Stride; }

    /// @brief  Gets the elements of the layout.
    /// @return The elements of the layout.
    inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }

private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int                     m_Stride;
};


}
