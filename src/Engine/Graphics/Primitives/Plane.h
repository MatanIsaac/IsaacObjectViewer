#pragma once

#include "Utility/config.h"
#include <memory>
#include "Engine/Graphics/Buffers/IndexBuffer.h"
#include "Engine/Graphics/Buffers/VertexArray.h"
#include "Engine/Graphics/Buffers/VertexBuffer.h"
#include "Engine/Graphics/Buffers/VertexBufferLayout.h"
#include "Engine/Graphics/Shaders/Shader.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "ISceneObject.h"
#include "Texture.h"
#include "Material.h"

namespace isaacObjectViewer
{
    class Plane : public ISceneObject
    {
    public:
        Plane(const glm::vec3& position = DEFAULT_POSITION);
        ~Plane() override;

        void Update();
        void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader = nullptr) override;
    
        std::size_t GetID() const { return m_ID; }
        const std::string& GetName() const override { return m_Name; };
        void SetName(const std::string& newName) override { m_Name = newName; }
        ObjectType GetType() const { return ObjectType::Plane; }

        glm::vec3& GetPosition() override { return m_Position; }
        glm::vec3& GetRotation() override { return m_Rotation; }
        glm::quat& GetOrientation() override { return m_Orientation; }
        glm::vec3& GetScale() override { return m_Scale; }
        glm::vec3& GetColor() { return m_Color; }
        float& GetShininess() override { return m_Material.Shininess; }
        const Material& GetMaterial() const { return m_Material; }

        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        void ResetRotation() { m_Rotation = DEFAULT_ROTATION; }
        void ResetScale() { m_Scale = DEFAULT_SCALE; }
        inline void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }
        inline void SetRotation(const glm::vec3& rotation) override { m_Rotation = rotation; }
        inline void SetOrientation(const glm::quat& newOrientation) override { m_Orientation = newOrientation; }
        inline void SetScale(const glm::vec3& scale) override { m_Scale = scale; }
        void SetColor(const glm::vec3& newColor) { m_Color = newColor; }
        void SetMaterial(const Material& newMaterial) { m_Material = newMaterial; }
        void SetDiffuseTexture(const std::shared_ptr<Texture>& tex) override { m_Material.Diffuse = tex; }
        void SetSpecularTexture(const std::shared_ptr<Texture>& tex) override { m_Material.Specular = tex; }

        inline const VertexArray    &GetVertexArray()   const override { return *m_VertexArray; }
        inline const VertexBuffer   &GetVertexBuffer()  const override { return *m_VertexBuffer; }
        inline const IndexBuffer    &GetIndexBuffer()   const override { return *m_IndexBuffer; }
        unsigned inline int         GetIndexCount()     const override { return m_IndicesCount; }
        inline int GetVertexCount() const { return m_VertexCount; }

        bool IntersectRay(const Ray& ray, float* outDistance) override
        {
            // 1. Compute plane normal (you should store this, or calculate from rotation)
            glm::vec3 normal = glm::vec3(0, 0, 1); // default (no rotation)
            // If you have m_Rotation, rotate the normal accordingly:
            glm::mat4 rotMat = glm::mat4(1.0f);
            rotMat = glm::rotate(rotMat, glm::radians(m_Rotation.x), glm::vec3(1,0,0));
            rotMat = glm::rotate(rotMat, glm::radians(m_Rotation.y), glm::vec3(0,1,0));
            rotMat = glm::rotate(rotMat, glm::radians(m_Rotation.z), glm::vec3(0,0,1));
            normal = glm::normalize(glm::vec3(rotMat * glm::vec4(normal, 0.0f)));

            float denom = glm::dot(normal, ray.GetDirection());
            if (fabs(denom) < 1e-6f) return false; // Ray is parallel to plane

            float t = glm::dot(m_Position - ray.GetOrigin(), normal) / denom;
            if (t < 0) return false; // Plane is behind ray

            // 2. Compute intersection point in world space
            glm::vec3 hitPoint = ray.GetOrigin() + t * ray.GetDirection();

            // 3. Transform hit point into plane's local space
            glm::mat4 invModel = glm::inverse(GetModelMatrix());
            glm::vec4 localHit = invModel * glm::vec4(hitPoint, 1.0f);

            // 4. Is localHit inside the quad? Plane is in XY, size 1x1
            if (localHit.x < -0.5f || localHit.x > 0.5f ||
                localHit.y < -0.5f || localHit.y > 0.5f)
                return false;

            if (outDistance) *outDistance = t;
            return true;
        }

        inline std::size_t GenerateUniqueID() override
        {
            static std::size_t currentID = 0;
            return ++currentID;
        }

    private:
        std::size_t m_ID;
        std::string m_Name;

        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::vec3 m_Scale;
        glm::vec3 m_Color;
        glm::quat m_Orientation;
        
        Material m_Material;

        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexArray> m_VertexArray;
        int m_VertexCount;
        static constexpr int m_FloatsPerVertex = 8;
        static constexpr int m_IndicesCount = 6;
    public:
        // Updated for 4 vertices (24 floats) for a simple quad
        static constexpr float m_PlaneVertices[32] = 
        { 
            // positions            // normals              // TexCoords
            -0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f,      0.0f, 0.0f, // bottom left
             0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f,      1.0f, 0.0f, // bottom right
             0.5f,  0.5f, 0.0f,      0.0f, 0.0f, 1.0f,      1.0f, 1.0f, // top right
            -0.5f,  0.5f, 0.0f,      0.0f, 0.0f, 1.0f,      0.0f, 1.0f, // top left
        };

        static constexpr unsigned int m_PlaneIndices[m_IndicesCount] =
        {
            0, 1, 2,
            2, 3, 0
        };
    };
}
