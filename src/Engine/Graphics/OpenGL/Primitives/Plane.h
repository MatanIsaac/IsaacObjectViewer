#pragma once

#include <memory>
#include "Utility/config.h"
#include "Engine/Graphics/OpenGL/Buffers/IndexBuffer.h"
#include "Engine/Graphics/OpenGL/Buffers/VertexArray.h"
#include "Engine/Graphics/OpenGL/Buffers/VertexBuffer.h"
#include "Engine/Graphics/OpenGL/Buffers/VertexBufferLayout.h"
#include "Engine/Graphics/OpenGL/Shaders/Shader.h"
#include "Engine/Graphics/OpenGL/Renderer/Renderer.h"

namespace isaacGraphicsEngine
{
    class Plane
    {
    public:
        Plane(const glm::vec3& position = {1.0f, 1.0f, 1.0f});
        ~Plane();

        void Update();
        void Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection);

        glm::vec3& GetPosition() { return m_Position; }
        glm::vec3& GetRotation() { return m_Rotation; }
        glm::vec3& GetScale() { return m_Scale; }
        glm::vec3& GetColor() { return m_Color; }
        glm::mat4 GetModelMatrix() const;

        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        void SetPosition(const glm::vec3& newPosition) { m_Position = newPosition; }
        void SetColor(const glm::vec3& newColor) { m_Color = newColor; }
        void SetScale(const glm::vec3& scale);
        void SetRotation(const glm::vec3& rotation);

        inline const VertexArray &GetPlaneVA() const { return *m_VertexArray; }
        inline const VertexBuffer &GetPlaneVB() const { return *m_VertexBuffer; }
        inline const IndexBuffer &GetPlaneIB() const { return *m_IndexBuffer; }
        inline int GetVertexCount() const { return m_VertexCount; }

    private:
        const glm::vec3 DEFAULT_POSITION = {1.0f, 1.0f, 1.0f};
        const glm::vec3 DEFAULT_COLOR = {1.0f, 0.5f, 0.31f};
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::vec3 m_Scale;
        glm::vec3 m_Color;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexArray> m_VertexArray;
        int m_VertexCount;
        static constexpr int m_FloatsPerVertex = 6;
        static constexpr int m_IndicesCount = 6;
    public:
        // Updated for 4 vertices (24 floats) for a simple quad
        static constexpr float m_PlaneVertices[24] = 
        { 
            // positions            // normals 
            -0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f, // bottom left
             0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f, // bottom right
             0.5f,  0.5f, 0.0f,      0.0f, 0.0f, 1.0f, // top right
            -0.5f,  0.5f, 0.0f,      0.0f, 0.0f, 1.0f, // top left
        };

        static constexpr unsigned int m_PlaneIndices[m_IndicesCount] =
        {
            0, 1, 2,
            2, 3, 0
        };
    };
}
