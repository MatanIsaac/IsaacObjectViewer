/**
 * @file VertexArray.h
 * @brief Header file for the VertexArray class.
 *  This class represents a vertex array in the graphics pipeline.
 *  It manages the storage and access of vertex data for rendering,
 *  And provides a method for binding and unbinding the vertex array.
 */

#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace isaacObjectViewer
{
class VertexArray
{
public:
    /// @brief Constructs a VertexArray.
    VertexArray();

    /// @brief Destroys the VertexArray.
    ~VertexArray();

    /// @brief Binds the VertexArray.
    void Bind() const;

    /// @brief Unbinds the VertexArray.
    void Unbind() const;

    /// @brief Adds a buffer to the VertexArray.
    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

private:
    unsigned int m_RendererID;
};
}