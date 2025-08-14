/**
 * @file Line.h
 * @brief Header file for the Line class.
 * This class represents a line primitive in the scene.
 * Note: Line does not inherit from IObject, like the rest of the primitives.
 * It handles its own rendering and is mainly used for debugging purposes (e.g., visualizing rays, normals, etc.).
 */

#pragma once

#include "Utility/config.h"
#include "Graphics/Shader/Shader.h"

namespace isaacObjectViewer
{
    class Line
    {
    public:

        /// @brief Constructs a Line.
        /// @param start The start point of the line.
        /// @param end The end point of the line.
        /// @param thickness The thickness of the line.
        /// @param color The color of the line.
        /// @param duration The duration for which the line should be visible.
        Line(const glm::vec3& start, const glm::vec3& end, float thickness = 4.0f, glm::vec3 color = {1.0f, 1.0f, 1.0f}, float duration = 5.0f);

        /// @brief Destroys the Line.
        ~Line();

        /// @brief Sets the thickness of the line.
        /// @param thickness The new thickness for the line.
        void SetThickness(float thickness);

        /// @brief Sets the color of the line.
        /// @param color The new color for the line.
        void SetColor(const glm::vec3& color);

        /// @brief Sets the duration for which the line should be visible.
        /// @param newDuration The new duration for the line.
        void SetDuration(float newDuration) { m_Duration = newDuration; }

        /// @brief Gets the duration for which the line is visible.
        /// @return The duration for which the line is visible.
        float GetDuration() const { return m_Duration; }

        /// @brief Gets the start point of the line.
        /// @return The start point of the line.
        const glm::vec3& GetStart() const { return m_Start; }

        /// @brief Gets the end point of the line.
        /// @return The end point of the line.
        const glm::vec3& GetEnd() const { return m_End; }

        /// @brief Gets the color of the line.
        /// @return The color of the line.
        const glm::vec3& GetColor() const { return m_Color; }

        /// @brief Gets the thickness of the line.
        /// @return The thickness of the line.
        float GetThickness() const { return m_Thickness; }

        /// @brief Renders the line.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param screenWidth The width of the screen.
        /// @param screenHeight The height of the screen.
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



        /// @brief Move constructor (deleted).
        Line(Line&& other) = delete;

        /// @brief Move assignment operator (deleted).
        Line& operator=(Line&& other) = delete;
    };
}