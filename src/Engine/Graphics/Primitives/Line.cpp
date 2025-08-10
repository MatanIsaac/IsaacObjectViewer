#include "Line.h"
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace isaacObjectViewer
{
    Line::Line(const glm::vec3& start, const glm::vec3& end, float thickness, glm::vec3 color, float duration)
        : m_Start(start), m_End(end), m_Thickness(thickness), m_Color(color), m_Duration(duration)
    {
        lineVertices.push_back(glm::vec4(m_Start, 1.0f));
        lineVertices.push_back(glm::vec4(m_End, 1.0f));

        glGenBuffers(1, &m_SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, lineVertices.size() * sizeof(glm::vec4), lineVertices.data(), GL_DYNAMIC_DRAW);

        // VAO is needed (but no attributes, we're using gl_VertexID)
        glGenVertexArrays(1, &m_VAO);

        std::string line_vs  = GetProjectRootPath("src/Resources/Shaders/line.vs");
        std::string line_fs  = GetProjectRootPath("src/Resources/Shaders/line.fs");

        // Compile shader from source (or load from file)
        m_ThickShader = new Shader(line_vs.c_str(), line_fs.c_str());
    }

    Line::~Line()
    {
        glDeleteBuffers(1, &m_SSBO);
        glDeleteVertexArrays(1, &m_VAO);
        delete m_ThickShader;
    }

    void Line::SetThickness(float thickness) { m_Thickness = thickness; }
    void Line::SetColor(const glm::vec3& color) { m_Color = color; }

    void Line::Render(const glm::mat4& view, const glm::mat4& projection, int screenWidth, int screenHeight)
    {
        m_ThickShader->Bind();

        // Model is identity
        glm::mat4 mvp = projection * view * glm::mat4(1.0f);
        m_ThickShader->setMat4("u_mvp", mvp);
        m_ThickShader->setVec2("u_resolution", glm::vec2(float(screenWidth), float(screenHeight)));
        m_ThickShader->setFloat("u_thickness", m_Thickness);
        m_ThickShader->setVec3("objectColor", m_Color);
        glBindVertexArray(m_VAO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO);

        // Draw 6 vertices (two triangles)
        glDrawArrays(GL_TRIANGLES, 0,  6);

        glBindVertexArray(0);
    }
}