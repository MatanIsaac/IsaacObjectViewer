#pragma once
#include "Utility/config.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Primitives/Line.h"

namespace isaacObjectViewer
{
    class Tracer
    {
    public:
        static Tracer* GetInstance()
        {
            if (s_Instance == nullptr)
                s_Instance = new Tracer();
            return s_Instance;
        }

        void AddLine(const glm::vec3& origin, const glm::vec3& dir, float duration, glm::vec3 color = {1,1,0}, float thickness = 4.0f);
        void Update(float deltaTime);
        void Render(const glm::mat4& view, const glm::mat4& projection, int w, int h);

    private:
        Tracer();
        ~Tracer();
        static Tracer* s_Instance;

        std::vector<std::unique_ptr<Line>> m_lines;
        std::vector<glm::vec4> m_vertexBuffer;
        GLuint m_SSBO = 0;
        GLuint m_VAO = 0;
        Shader* m_ThickShader = nullptr;
    };
}