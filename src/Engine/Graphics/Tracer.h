/**
 * @file Tracer.h
 * @brief Header file for the Tracer class.
 * This class is responsible for rendering debug lines in the scene.
 */

#pragma once
#include "Utility/config.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Primitives/Line.h"

namespace isaacObjectViewer
{
    class Tracer
    {
    public:
        /// @brief Gets the singleton instance of the Tracer.
        /// @return The singleton instance of the Tracer.
        static Tracer* GetInstance()
        {
            if (s_Instance == nullptr)
                s_Instance = new Tracer();
            return s_Instance;
        }

        /// @brief Adds a line to the tracer.
        /// @param origin The origin of the line.
        /// @param dir The direction of the line.
        /// @param duration The duration of the line.
        /// @param color The color of the line.
        /// @param thickness The thickness of the line.
        void AddLine(const glm::vec3& origin, const glm::vec3& dir, float duration, glm::vec3 color = {1,1,0}, float thickness = 4.0f);

        /// @brief Updates the tracer lines.
        /// @param deltaTime The time elapsed since the last update.
        void Update(float deltaTime);

        /// @brief Renders the tracer lines.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param w The width of the viewport.
        /// @param h The height of the viewport.
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