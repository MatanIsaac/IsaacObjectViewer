#include "VertexBufferLayout.h"

namespace isaacGraphicsEngine
{

    VertexBufferLayout::VertexBufferLayout()
        : m_Stride(0)
    {
    }

    template <>
    void VertexBufferLayout::Push<float>(unsigned int count)
    {
        m_Elements.push_back({count, GL_FLOAT, false});
        m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
    }

    template <>
    void VertexBufferLayout::Push<unsigned int>(unsigned int count)
    {
        m_Elements.push_back({count, GL_UNSIGNED_INT, false});
        m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
    }

    template <>
    void VertexBufferLayout::Push<unsigned char>(unsigned int count)
    {
        m_Elements.push_back({count, GL_UNSIGNED_BYTE, true});
        m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
    }
}
