#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Utility/config.h"
#include "Engine/Graphics/OpenGL/Buffers/IndexBuffer.h"
#include "Engine/Graphics/OpenGL/Buffers/VertexArray.h"
#include "Engine/Graphics/OpenGL/Buffers/VertexBuffer.h"
#include "Engine/Graphics/OpenGL/Buffers/VertexBufferLayout.h"
#include "Engine/Graphics/OpenGL/Shaders/Shader.h"
#include "Engine/Graphics/OpenGL/Renderer/Renderer.h"

namespace isaacGraphicsEngine
{
    class Sphere
    {
    public:
        // The default position is left as before.
        Sphere(const glm::vec3& position = {1.0f, 1.0f, 1.0f});
        ~Sphere();

        void Update();
        void Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection);

        glm::vec3& GetPosition() { return m_Position; }
        glm::vec3& GetColor()    { return m_Color; }

        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        void SetPosition(const glm::vec3& newPosition) { m_Position = newPosition; }
        void SetColor(const glm::vec3& newColor) { m_Color = newColor; }

        inline const VertexArray& GetVertexArray() const { return *m_VertexArray; }
        inline const IndexBuffer& GetIndexBuffer() const { return *m_IndexBuffer; }
        inline unsigned int GetVertexCount() const { return m_VertexCount; }
        inline unsigned int GetIndexCount() const { return m_IndexCount; }

    private:
        const glm::vec3 DEFAULT_POSITION = {1.0f, 1.0f, 1.0f};
        const glm::vec3 DEFAULT_COLOR = {1.0f, 0.5f, 0.31f};

        glm::vec3 m_Position;
        glm::vec3 m_Scale;
        glm::vec3 m_Color;

        std::unique_ptr<VertexArray> m_VertexArray;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;

        glm::mat4 m_ModelMatrix;

        int m_VertexCount;
        unsigned int m_IndexCount;

        // Modified: Generates vertices and indices for a sphere using a radius,
        // sectorCount (longitude divisions) and stackCount (latitude divisions).
        // Each vertex contains 6 floats (position and normal).
        int GenerateSphere(float radius, int sectorCount, int stackCount,
                           std::vector<float>& vertices, std::vector<unsigned int>& indices);
    };
} // namespace isaacGraphicsEngine
