#pragma once

#include "Utility/config.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Engine/Graphics/Buffers/IndexBuffer.h"
#include "Engine/Graphics/Buffers/VertexArray.h"
#include "Engine/Graphics/Buffers/VertexBuffer.h"
#include "Engine/Graphics/Buffers/VertexBufferLayout.h"
#include "Engine/Graphics/Shaders/Shader.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Scene/ISceneObject.h"
#include "Texture.h"
#include "Material.h"


namespace isaacObjectViewer
{
    class Sphere : public ISceneObject
    {
    public:
        // The default position is left as before.
        Sphere(const glm::vec3& position = DEFAULT_POSITION);
        ~Sphere() override;

        void Update();
        void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader = nullptr) override;
        
        std::size_t GetID() const { return m_ID; }
        const std::string& GetName() const override { return m_Name; };
        void SetName(const std::string& newName) override { m_Name = newName; }
        ObjectType GetType() const { return ObjectType::Sphere; }

        glm::vec3& GetPosition() override { return m_Position; }
        glm::vec3& GetRotation() override { return m_Rotation; }
        glm::quat& GetOrientation() override { return m_Orientation; }
        glm::vec3& GetScale() override { return m_Scale; }
        glm::vec3& GetColor()    { return m_Color; }
        float& GetShininess() override { return m_Material.Shininess; }
        const Material& GetMaterial() const { return m_Material; }

        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        void ResetRotation() { m_Rotation = DEFAULT_ROTATION; }
        void ResetScale() { m_Scale = DEFAULT_SCALE; }
        inline void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }
        inline void SetRotation(const glm::vec3& newRotation) override { m_Rotation = newRotation; }
        inline void SetOrientation(const glm::quat& newOrientation) override { m_Orientation = newOrientation; }
        inline void SetScale(const glm::vec3& newScale) override { m_Scale = newScale; }
        inline void SetColor(const glm::vec3& newColor) { m_Color = newColor; }
        void SetMaterial(const Material& newMaterial) { m_Material = newMaterial; }
        void SetDiffuseTexture(const std::shared_ptr<Texture>& tex) override { }
        void SetSpecularTexture(const std::shared_ptr<Texture>& tex) override { }
        
        inline const VertexArray    &GetVertexArray()   const override { return *m_VertexArray; }
        inline const VertexBuffer   &GetVertexBuffer()  const override { return *m_VertexBuffer; }
        inline const IndexBuffer    &GetIndexBuffer()   const override { return *m_IndexBuffer; }
        unsigned inline int         GetIndexCount()     const override { return m_IndexCount; }

        bool IntersectRay(const Ray& ray, float* outDistance) override
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

        std::unique_ptr<VertexArray> m_VertexArray;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;

        int m_VertexCount;
        unsigned int m_IndexCount;

        // Generates vertices and indices for a sphere using a radius,
        // sectorCount (longitude divisions) and stackCount (latitude divisions).
        // Each vertex contains 6 floats (position and normal).
        int GenerateSphere(float radius, int sectorCount, int stackCount,
                           std::vector<float>& vertices, std::vector<unsigned int>& indices);
    };
} // namespace isaacGraphicsEngine
