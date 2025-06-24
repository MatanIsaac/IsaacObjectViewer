#pragma once

#include <memory>
#include "Utility/config.h"

#include "Engine/Graphics/OpenGL/Buffers/IndexBuffer.h"
#include "Engine/Graphics/OpenGL/Buffers/VertexArray.h"
#include "Engine/Graphics/OpenGL/Buffers/VertexBuffer.h"
#include "Engine/Graphics/OpenGL/Buffers/VertexBufferLayout.h"
#include "Engine/Graphics/OpenGL/Shaders/Shader.h"
#include "Engine/Graphics/OpenGL/Renderer/Renderer.h"
#include "IPrimitive.h"

namespace isaacObjectLoader
{
    class Cube : public IPrimitive
    {
    public:
        Cube(const glm::vec3& position = {1.0f, 1.0f, 1.0f});
        ~Cube() override;

        void Update();
        void Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection);

        virtual glm::vec3& GetPosition() override { return m_Position; }
        virtual glm::vec3& GetRotation() override { return m_Rotation; }
        virtual glm::vec3& GetScale() override { return m_Scale; }
        glm::vec3& GetColor() { return m_Color; }
        glm::mat4 GetModelMatrix() const;

        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        void ResetRotation() { m_Rotation = DEFAULT_ROTATION; }
        void ResetScale() { m_Scale = DEFAULT_SCALE; }
        void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }
        void SetRotation(const glm::vec3& rotation) override;
        void SetScale(const glm::vec3& scale) override;
        void SetColor(const glm::vec3& newColor) { m_Color = newColor; }

        inline const VertexArray &GetCubeVA() const { return *m_VertexArray; }
        inline const VertexBuffer &GetCubeVB() const { return *m_VertexBuffer; }
        inline const IndexBuffer &GetCubeIB() const { return *m_IndexBuffer; }
        inline int GetIndexCount() const { return m_IndicesCount; }

        bool IntersectRay(const Ray& ray, float* outDist) const override
        {
            glm::vec3 halfScale = m_Scale * 0.5f;
            glm::vec3 boxMin = m_Position - halfScale;
            glm::vec3 boxMax = m_Position + halfScale;
            return RayIntersectsAABB(ray, boxMin, boxMax, outDist);
        }


    private:
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::vec3 m_Scale;
        glm::vec3 m_Color;

        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexArray> m_VertexArray;

        int m_IndicesCount; // will be set to 36 (6 faces * 6 indices)

        static constexpr int m_FloatsPerVertex = 6;

    public:
        // 24 vertices (4 per face) × 6 floats each = 144 floats.
        // Each vertex: position (x,y,z) then normal (x,y,z)
        static constexpr float m_CubeVertices[144] = 
        {
            // Front face (z = -0.5, normal (0,0,-1))
            -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  // bottom-left
             0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  // bottom-right
             0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  // top-right
            -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  // top-left

            // Back face (z = 0.5, normal (0,0,1))
            -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  // bottom-left
             0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  // bottom-right
             0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  // top-right
            -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  // top-left

            // Left face (x = -0.5, normal (-1,0,0))
            -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  // bottom-left
            -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  // bottom-right
            -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  // top-right
            -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  // top-left

            // Right face (x = 0.5, normal (1,0,0))
             0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  // bottom-left
             0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  // bottom-right
             0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  // top-right
             0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  // top-left

            // Bottom face (y = -0.5, normal (0,-1,0))
            -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  // bottom-left
             0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  // bottom-right
             0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  // top-right
            -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  // top-left

            // Top face (y = 0.5, normal (0,1,0))
            -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  // bottom-left
             0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  // bottom-right
             0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  // top-right
            -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f   // top-left
        };

        // For each face (4 vertices) we define two triangles: (0,1,2) and (2,3,0)
        // Since vertices for each face are sequential, we offset per face.
        static constexpr unsigned int m_CubeIndices[36] =
        {
            // Front face
             0,  1,  2,
             2,  3,  0,
            // Back face
             4,  5,  6,
             6,  7,  4,
            // Left face
             8,  9, 10,
            10, 11,  8,
            // Right face
            12, 13, 14,
            14, 15, 12,
            // Bottom face
            16, 17, 18,
            18, 19, 16,
            // Top face
            20, 21, 22,
            22, 23, 20
        };
    };
} // namespace isaacGraphicsEngine
