#include "Tracer.h"
#include "Utility/config.h"
#include <glm/gtc/type_ptr.hpp>

namespace isaacObjectViewer
{
    Tracer* Tracer::s_Instance = nullptr;

    Tracer::Tracer()
    {
        glGenBuffers(1, &m_SSBO);
        glGenVertexArrays(1, &m_VAO);

        std::string line_vs = GetProjectRootPath("src/Resources/Shaders/line.vs");
        std::string line_fs = GetProjectRootPath("src/Resources/Shaders/line.fs");
        m_ThickShader = new Shader(line_vs.c_str(), line_fs.c_str());
    }

    Tracer::~Tracer()
    {
        glDeleteBuffers(1, &m_SSBO);
        glDeleteVertexArrays(1, &m_VAO);
        delete m_ThickShader;
    }

    void Tracer::AddLine(const glm::vec3& origin, const glm::vec3& dir, float duration, glm::vec3 color, float thickness)
    {
        m_lines.push_back(std::make_unique<Line>(origin, origin + dir, thickness, color, duration));
    }

    void Tracer::Update(float deltaTime)
    {
        for (auto it = m_lines.begin(); it != m_lines.end(); )
        {
            (*it)->SetDuration((*it)->GetDuration() - deltaTime);
            if ((*it)->GetDuration() <= 0)
                it = m_lines.erase(it);
            else
                ++it;
        }
    }

    void Tracer::Render(const glm::mat4& view, const glm::mat4& projection, int w, int h)
    {
        if (m_lines.empty())
            return;

        for (auto& line : m_lines)
        {
            line->Render(view, projection, w, h);
        }
    }
}