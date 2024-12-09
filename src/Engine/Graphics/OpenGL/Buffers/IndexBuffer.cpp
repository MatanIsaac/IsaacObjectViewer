
#include "IndexBuffer.h"
#include "Utility/config.h"

namespace isaacObjectLoader
{

IndexBuffer::IndexBuffer(const void* data, unsigned int count)
: m_Count(count)
{
    assert(sizeof(unsigned int) == sizeof(GLuint));

    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &m_RendererID); }

void IndexBuffer::Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID); }

void IndexBuffer::Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
}
