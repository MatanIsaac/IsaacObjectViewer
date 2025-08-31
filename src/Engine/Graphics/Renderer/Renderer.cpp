#include "Renderer.h"
#include "Utility/GLErrorManager.h"

namespace isaacObjectViewer
{


    void Renderer::Clear() const { GLCall(glClear(GL_COLOR_BUFFER_BIT)); }

    void Renderer::Render(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
    {
        shader.Bind();
        va.Bind();
        ib.Bind();
        GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
        ++m_FrameStats.DrawCalls;
        m_FrameStats.Vertices  += static_cast<uint64_t>(ib.GetCount());
        m_FrameStats.Triangles += static_cast<uint64_t>(ib.GetCount() / 3);
    }

    void Renderer::Render(const VertexArray& va, int count, const Shader& shader) const
    {
        shader.Bind();
        va.Bind();
        GLCall(glDrawArrays(GL_TRIANGLES, 0, count));

        ++m_FrameStats.DrawCalls;
        m_FrameStats.Vertices  += static_cast<uint64_t>(count);
        m_FrameStats.Triangles += static_cast<uint64_t>(count / 3);
    }

    void Renderer::BeginFrame() { m_FrameStats.reset();}

}