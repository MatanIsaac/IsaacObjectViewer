#include "VertexArray.h"
#include "Utility/GLErrorManager.h"

namespace isaacObjectLoader
{
VertexArray::VertexArray() { GLCall(glGenVertexArrays(1, &m_RendererID)); }
VertexArray::~VertexArray() { GLCall(glDeleteVertexArrays(1, &m_RendererID)); }

void VertexArray::Bind() const { GLCall(glBindVertexArray(m_RendererID)); }

void VertexArray::Unbind() const { GLCall(glBindVertexArray(0)); }

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
    Bind();
    vb.Bind();
    const auto&  elements = layout.GetElements();
    unsigned int offset   = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        // Enable and specify the vertex attribute pointer
        GLboolean normalized = (element.m_Normalized) ? GL_TRUE : GL_FALSE;
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(
            i,
            element.m_Count,
            element.m_Type,
            normalized,
            layout.GetStride(),
            reinterpret_cast<const void*>(static_cast<std::intptr_t>(offset))));
        offset += element.m_Count * VertexBufferElement::GetSizeOfType(element.m_Type);
    }
}
}  // namespace isaacObjectLoader