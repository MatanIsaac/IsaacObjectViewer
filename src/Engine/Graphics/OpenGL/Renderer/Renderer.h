#pragma once

#include "Engine/Graphics/OpenGL/Buffers/IndexBuffer.h"
#include "Engine/Graphics/OpenGL/Shaders/Shader.h"
#include "Engine/Graphics/OpenGL/Buffers/VertexArray.h"

namespace isaacObjectLoader
{
class Renderer
{
public:
    void Clear() const;
    // Draws elements
    void Render(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    // Draws arrays
    void Render(const VertexArray& va, int count, const Shader& shader) const;

private:
};

}  // namespace isaacObjectLoader
