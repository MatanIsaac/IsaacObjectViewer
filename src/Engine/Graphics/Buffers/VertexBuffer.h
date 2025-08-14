/**
 * @file VertexBuffer.h
 * @brief Header file for the VertexBuffer class.
 *  This class represents a vertex buffer in the graphics pipeline.
 *  It manages the storage and access of vertex data for rendering,
 *  And provides a method for binding and unbinding the vertex buffer.
 */

#pragma once

namespace isaacObjectViewer
{
class VertexBuffer
{
public:
    /// @brief Creates a vertex buffer object
    /// @param data pointer to the vertex data
    /// @param size the size in bytes
    VertexBuffer(const void* data, unsigned int size);

    /// @brief Destroys the VertexBuffer.
    ~VertexBuffer();

    /// @brief Binds the VertexBuffer.
    void Bind() const;

    /// @brief Unbinds the VertexBuffer.
    void Unbind() const;

    /// @brief Gets the renderer ID of the VertexBuffer.
    /// @return The renderer ID.
    int  getRendererID() const { return m_RendererID; }

private:
    unsigned int m_RendererID;
};
}