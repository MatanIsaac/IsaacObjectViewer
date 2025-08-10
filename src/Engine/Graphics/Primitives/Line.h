#pragma once

#include "Utility/config.h"
#include "Graphics/Shaders/Shader.h"

namespace isaacObjectViewer
{
    class Line
    {
    public:
        Line(const glm::vec3& start, const glm::vec3& end, float thickness = 4.0f, glm::vec3 color = {1.0f, 1.0f, 1.0f}, float duration = 5.0f);
       ~Line();

        void SetThickness(float thickness);
        void SetColor(const glm::vec3& color);
        void SetDuration(float newDuration) { m_Duration = newDuration; }
        float GetDuration() const { return m_Duration; }
        const glm::vec3& GetStart() const { return m_Start; }
        const glm::vec3& GetEnd() const { return m_End; }
        const glm::vec3& GetColor() const { return m_Color; }
        float GetThickness() const { return m_Thickness; }

        void Render(const glm::mat4& view, const glm::mat4& projection, int screenWidth, int screenHeight);
    private:
        GLuint m_SSBO = 0;
        GLuint m_VAO = 0;
        glm::vec3 m_Start, m_End;
        float m_Thickness;
        glm::vec3 m_Color;
        Shader* m_ThickShader;
        float m_Duration;
        std::vector<glm::vec4> lineVertices;
        // Disable copy and move
        Line(const Line&) = delete;
        Line& operator=(const Line&) = delete;
        Line(Line&&) = delete;
        Line& operator=(Line&&) = delete;
    };
}