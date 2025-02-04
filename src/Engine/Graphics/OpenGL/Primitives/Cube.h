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
    class Cube
    {
    public:
        Cube(const glm::vec3& position = {1.0f, 1.0f, 1.0f});
        ~Cube();

        void Update();
        void Render(const Renderer& renderer,Shader& shader, const glm::mat4& view, const glm::mat4& projection);

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

        inline const VertexArray &GetCubeVA() const { return *m_VertexArray; }
        inline const VertexBuffer &GetCubeVB() const { return *m_VertexBuffer; }
        inline const IndexBuffer &GetCubeIB() const { return *m_IndexBuffer; }
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
        int m_IndicesCount;
        int m_FloatsPerVertex = 6;
    public:
        static constexpr float m_CubeVertices[216] = 
        { 
            // positions            // normals 

            // Front face
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f, // 0
             0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f, // 1
             0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f, // 2
             0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f, // 3
            -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f, // 4
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f, // 5

            // Back face
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  1.0f, // 6
             0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  1.0f, // 7
             0.5f,  0.5f,  0.5f,    0.0f, 0.0f,  1.0f, // 8
             0.5f,  0.5f,  0.5f,    0.0f, 0.0f,  1.0f, // 9
            -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,  1.0f, // 10
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,  1.0f, // 11

            // Left face
            -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f,  0.0f, // 12 
            -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f,  0.0f, // 13
            -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f,  0.0f, // 14
            -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f,  0.0f, // 15
            -0.5f, -0.5f,  0.5f,   -1.0f, 0.0f,  0.0f, // 16
            -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f,  0.0f, // 17

            // Right face
            0.5f,  0.5f,  0.5f,    1.0f, 0.0f,  0.0f, // 18
            0.5f,  0.5f, -0.5f,    1.0f, 0.0f,  0.0f, // 19
            0.5f, -0.5f, -0.5f,    1.0f, 0.0f,  0.0f, // 20
            0.5f, -0.5f, -0.5f,    1.0f, 0.0f,  0.0f, // 21
            0.5f, -0.5f,  0.5f,    1.0f, 0.0f,  0.0f, // 22
            0.5f,  0.5f,  0.5f,    1.0f, 0.0f,  0.0f, // 23

            // Bottom face
            -0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,

            // Top face
            -0.5f,  0.5f, -0.5f,    0.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,    0.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,    0.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,    0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,    0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,    0.0f,  1.0f, 0.0f
        };
    };
} // namespace isaacGraphicsEngine
