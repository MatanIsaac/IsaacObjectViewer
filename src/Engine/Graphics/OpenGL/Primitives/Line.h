#pragma once
#include <glm/glm.hpp>
#include "Engine/Graphics/OpenGL/Shaders/Shader.h"

namespace isaacObjectLoader
{
    class Line
    {
    public:
        Line(const glm::vec3& start, const glm::vec3& end, float thickness = 4.0f, glm::vec3 color = {1.0f, 1.0f, 1.0f});
        ~Line();

        void SetEndpoints(const glm::vec3& start, const glm::vec3& end);
        void SetThickness(float thickness);
        void SetColor(const glm::vec3& color);

        void Render(const glm::mat4& view, const glm::mat4& projection, int screenWidth, int screenHeight);

    private:
        GLuint m_SSBO = 0;
        GLuint m_VAO = 0;
        glm::vec3 m_Start, m_End;
        float m_Thickness;
        glm::vec3 m_Color;
        Shader* m_ThickShader;
    };
}
