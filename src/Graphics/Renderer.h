#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"

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
