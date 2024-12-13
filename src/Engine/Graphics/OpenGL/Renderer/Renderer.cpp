#include "Renderer.h"
#include "Utility/GLErrorManager.h"

namespace isaacGraphicsEngine
{


void Renderer::Clear() const { GLCall(glClear(GL_COLOR_BUFFER_BIT)); }

void Renderer::Render(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Render(const VertexArray& va, int count, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    GLCall(glDrawArrays(GL_TRIANGLES, 0, count));
}

}