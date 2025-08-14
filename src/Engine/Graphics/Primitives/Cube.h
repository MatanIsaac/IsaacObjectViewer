#pragma once

#include "Utility/config.h"
#include <memory>

#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Renderer/Renderer.h"
#include "ISceneObject.h"
#include "Texture.h"
#include "Material.h"

namespace isaacObjectViewer
{
    class Cube : public ISceneObject
    {
    public:
        Cube(const glm::vec3& position = DEFAULT_POSITION);
        ~Cube() override;

        void Update();
        void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader = nullptr) override;
    
        std::size_t GetID() const { return m_ID; }
        const std::string& GetName() const override { return m_Name; };
        void SetName(const std::string& newName) override { m_Name = newName; }
        ObjectType GetType() const { return ObjectType::Cube; }

        glm::vec3& GetPosition() override { return m_Position; }
        glm::vec3& GetRotation() override { return m_Rotation; }
        glm::quat& GetOrientation() override { return m_Orientation; }
        glm::vec3& GetScale() override { return m_Scale; }
        glm::vec3& GetColor() override{ return m_Color; }
        bool& GetUseMaterial() override { return m_UseMaterial; }
        float& GetShininess() override { return m_Material.Shininess; }
        const Material& GetMaterial() const { return m_Material; }

        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        void ResetRotation() { m_Rotation = DEFAULT_ROTATION; }
        void ResetScale() { m_Scale = DEFAULT_SCALE; }
        inline void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }
        inline void SetRotation(const glm::vec3& rotation) override { m_Rotation = rotation; }
        inline void SetOrientation(const glm::quat& newOrientation) override { m_Orientation = newOrientation; }
        inline void SetScale(const glm::vec3& scale) override { m_Scale = scale; }
        inline void SetColor(const glm::vec3& newColor) override{ m_Color = newColor; }
        inline void SetUseMaterial(bool use) override { m_UseMaterial = use; }
        void SetMaterial(const Material& newMaterial) { m_Material = newMaterial; }
        void SetDiffuseTexture(const std::shared_ptr<Texture>& tex) override { m_Material.Diffuse = tex; }
        void SetSpecularTexture(const std::shared_ptr<Texture>& tex) override { m_Material.Specular = tex; }

        inline const VertexArray    &GetVertexArray()   const override { return *m_VertexArray; }
        inline const VertexBuffer   &GetVertexBuffer()  const override { return *m_VertexBuffer; }
        inline const IndexBuffer    &GetIndexBuffer()   const override { return *m_IndexBuffer; }
        inline unsigned int         GetIndexCount()     const override { return m_IndicesCount; }
        inline unsigned int         GetVertexCount() const override { return m_VertexCount; }

        bool IntersectRay(const Ray& ray, float* outDist) override
        {
            glm::vec3 halfScale = m_Scale * 0.5f;
            glm::vec3 boxMin = m_Position - halfScale;
            glm::vec3 boxMax = m_Position + halfScale;
            return RayIntersectsAABB(ray, boxMin, boxMax, outDist);
        }

        inline std::size_t GenerateUniqueID() override
        {
            static std::size_t cubeID = 0;
            return ++cubeID;
        }
        
    private:
        std::size_t m_ID;
        std::string m_Name;
        
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::quat m_Orientation;
        glm::vec3 m_Scale;
        glm::vec3 m_Color;
        bool m_UseMaterial;

        Material m_Material;

        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexArray> m_VertexArray;

        int m_IndicesCount; // will be set to 36 (6 faces * 6 indices)

        static constexpr int m_FloatsPerVertex = 8;
        unsigned int m_VertexCount = 24; 

    public:
        // 24 vertices (4 per face) × 6 floats each = 144 floats.
        // Each vertex: position (x,y,z) then normal (x,y,z)
        static constexpr float m_CubeVertices[192] = 
        {
            // Front face (z = -0.5, normal (0,0,-1))
            -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // bottom-left
             0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // bottom-right
             0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // top-right
            -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // top-left

            // Back face (z = 0.5, normal (0,0,1))
            -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f, // bottom-left
             0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f, // bottom-right
             0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f, // top-right
            -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 1.0f, // top-left

            // Left face (x = -0.5, normal (-1,0,0))
            -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // bottom-right
            -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-right
            -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // top-left

            // Right face (x = 0.5, normal (1,0,0))
             0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-left
             0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // bottom-right
             0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-right
             0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // top-left

            // Bottom face (y = -0.5, normal (0,-1,0))
            -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f, // bottom-left
             0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
             0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f, // top-right
            -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f, // top-left

            // Top face (y = 0.5, normal (0,1,0))
            -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f, // bottom-left
             0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
             0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // top-right
            -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f // top-left
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
