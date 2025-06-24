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
    class Plane : public IPrimitive
    {
    public:
        Plane(const glm::vec3& position = {1.0f, 1.0f, 1.0f});
        ~Plane() override;

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
        virtual void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }
        virtual void SetRotation(const glm::vec3& rotation) override;
        virtual void SetScale(const glm::vec3& scale) override;
        void SetColor(const glm::vec3& newColor) { m_Color = newColor; }

        inline const VertexArray &GetPlaneVA() const { return *m_VertexArray; }
        inline const VertexBuffer &GetPlaneVB() const { return *m_VertexBuffer; }
        inline const IndexBuffer &GetPlaneIB() const { return *m_IndexBuffer; }
        inline int GetVertexCount() const { return m_VertexCount; }

        bool IntersectRay(const Ray& ray, float* outDistance) const override
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



    private:
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
