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
#include "IPrimitive.h"

namespace isaacObjectLoader
{
    class Sphere : public IPrimitive
    {
    public:
        // The default position is left as before.
        Sphere(const glm::vec3& position = {1.0f, 1.0f, 1.0f});
        ~Sphere() override;

        void Update();
        void Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection);

        virtual glm::vec3& GetPosition() override { return m_Position; }
        virtual glm::vec3& GetRotation() override { return m_Rotation; }
        virtual glm::vec3& GetScale() override { return m_Scale; }
        glm::vec3& GetColor()    { return m_Color; }
        glm::mat4 GetModelMatrix() const;

        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        void ResetRotation() { m_Rotation = DEFAULT_ROTATION; }
        void ResetScale() { m_Scale = DEFAULT_SCALE; }
        void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }
        void SetRotation(const glm::vec3& newRotation) override { m_Rotation = newRotation; }
        void SetScale(const glm::vec3& newScale) override { m_Scale = newScale; }
        void SetColor(const glm::vec3& newColor) { m_Color = newColor; }

        inline const VertexArray& GetVertexArray() const { return *m_VertexArray; }
        inline const IndexBuffer& GetIndexBuffer() const { return *m_IndexBuffer; }
        inline unsigned int GetVertexCount() const { return m_VertexCount; }
        inline unsigned int GetIndexCount() const { return m_IndexCount; }

        bool IntersectRay(const Ray& ray, float* outDistance) const override
        {
            // Assume uniform scale, use x
            float worldRadius = 0.5f * m_Scale.x; // Your generated sphere is radius 0.5f
            glm::vec3 oc = ray.GetOrigin() - m_Position;

            float a = glm::dot(ray.GetDirection(), ray.GetDirection());
            float b = 2.0f * glm::dot(oc, ray.GetDirection());
            float c = glm::dot(oc, oc) - worldRadius * worldRadius;
            float discriminant = b * b - 4 * a * c;
            if (discriminant < 0) return false;
            float sqrtDisc = sqrt(discriminant);
            float t1 = (-b - sqrtDisc) / (2.0f * a);
            float t2 = (-b + sqrtDisc) / (2.0f * a);
            float t = (t1 > 0) ? t1 : t2;
            if (t < 0) return false; // Intersection is behind ray origin
            if (outDistance) *outDistance = t;
            return true;
        }


    private:
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::vec3 m_Scale;
        glm::vec3 m_Color;

        std::unique_ptr<VertexArray> m_VertexArray;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;

        int m_VertexCount;
        unsigned int m_IndexCount;

        // Modified: Generates vertices and indices for a sphere using a radius,
        // sectorCount (longitude divisions) and stackCount (latitude divisions).
        // Each vertex contains 6 floats (position and normal).
        int GenerateSphere(float radius, int sectorCount, int stackCount,
                           std::vector<float>& vertices, std::vector<unsigned int>& indices);
    };
} // namespace isaacGraphicsEngine
