/**
 * @file Renderer.h
 * @brief Header file for the Renderer class.
 * This class is responsible for rendering 3D objects.
 * Has Two main functions: Clear and Render.
 * Clear: Clears the screen.
 * Render: Renders the 3D objects, has two overloads.
 *   - One for rendering indexed geometry.
 *   - Another for rendering non-indexed geometry.
 */

#pragma once

#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Buffers/VertexArray.h"

namespace isaacObjectViewer
{
class Renderer
{
public:
    /// @brief Clears the screen.
    void Clear() const;

    /// @brief Renders indexed geometry.
    /// @param va The vertex array to render.
    /// @param ib The index buffer to use.
    /// @param shader The shader to use.
    void Render(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

    /// @brief Renders non-indexed geometry.
    /// @param va The vertex array to render.
    /// @param count The number of vertices to render.
    /// @param shader The shader to use.
    void Render(const VertexArray& va, int count, const Shader& shader) const;

private:
};

}  // namespace isaacGraphicsEngine
