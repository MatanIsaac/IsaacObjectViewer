#pragma once

namespace isaacObjectLoader
{
class VertexBuffer
{
public:
    /// @brief Creates a vertex buffer object
    /// @param data pointer to the vertex data
    /// @param size the size in bytes
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
    int  getRendererID() const { return m_RendererID; }

private:
    unsigned int m_RendererID;
};
}