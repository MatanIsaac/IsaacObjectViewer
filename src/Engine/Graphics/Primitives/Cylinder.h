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
#include "ISceneObject.h"
#include "Material.h"

namespace isaacObjectViewer
{
    class Cylinder : public ISceneObject
    {
    public:
        Cylinder(const glm::vec3& position = DEFAULT_POSITION);
        ~Cylinder() override;

        void Update();
        void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader = nullptr) override;
        
        std::size_t GetID() const { return m_ID; }
        const std::string& GetName() const override { return m_Name; };
        void SetName(const std::string& newName) override { m_Name = newName; }
        ObjectType GetType() const { return ObjectType::Cylinder; }
        
        glm::vec3& GetPosition() override { return m_Position; }
        glm::vec3& GetRotation() override { return m_Rotation; }
        glm::quat& GetOrientation() override { return m_Orientation; }
        glm::vec3& GetScale() override { return m_Scale; }
        glm::vec3& GetColor() override { return m_Color; }
        bool& GetUseMaterial() override { return m_UseMaterial; }
        float& GetShininess() override { return m_Material.Shininess; }
        const Material& GetMaterial() const { return m_Material; }

        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        void ResetRotation() { m_Rotation = DEFAULT_ROTATION; }
        void ResetScale() { m_Scale = DEFAULT_SCALE; }
        inline void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }
        inline void SetRotation(const glm::vec3& newRotation) override { m_Rotation = newRotation; }
        inline void SetOrientation(const glm::quat& newOrientation) override { m_Orientation = newOrientation; }
        inline void SetScale(const glm::vec3& newScale) override { m_Scale = newScale; }
        inline void SetColor(const glm::vec3& newColor) override { m_Color = newColor; }
        inline void SetUseMaterial(bool useMaterial) override { m_UseMaterial = useMaterial; }
        void SetMaterial(const Material& newMaterial) { m_Material = newMaterial; }
        void SetDiffuseTexture(const std::shared_ptr<Texture>& tex) override { }
        void SetSpecularTexture(const std::shared_ptr<Texture>& tex) override { }

        inline const VertexArray    &GetVertexArray()   const override { return *m_VertexArray; }
        inline const VertexBuffer   &GetVertexBuffer()  const override { return *m_VertexBuffer; }
        inline const IndexBuffer    &GetIndexBuffer()   const override { return *m_IndexBuffer; }
        unsigned inline int         GetIndexCount()     const override { return m_IndexCount; }
        inline unsigned int GetVertexCount() const override{ return m_VertexCount; }
        

        bool IntersectRay(const Ray& ray, float* outDistance) override
        {
            // 1. Inverse-transform the ray to object (local) space
            glm::mat4 invModel = glm::inverse(GetModelMatrix());
            glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(ray.GetOrigin(), 1.0f));
            glm::vec3 localDir = glm::normalize(glm::vec3(invModel * glm::vec4(ray.GetDirection(), 0.0f)));

            // 2. Side intersection: infinite cylinder along Z axis, radius 0.5, height 1 (-0.5 to +0.5)
            float a = localDir.x * localDir.x + localDir.y * localDir.y;
            float b = 2.0f * (localOrigin.x * localDir.x + localOrigin.y * localDir.y);
            float c = localOrigin.x * localOrigin.x + localOrigin.y * localOrigin.y - 0.25f; // (0.5f^2)

            float discriminant = b * b - 4 * a * c;
            float tSide = -1.0f;

            // Hit side?
            if (discriminant >= 0.0f && fabs(a) > 1e-6f)
            {
                float sqrtDisc = sqrt(discriminant);
                float t1 = (-b - sqrtDisc) / (2.0f * a);
                float t2 = (-b + sqrtDisc) / (2.0f * a);

                // We want the nearest positive t where the hit is within the caps
                for (float tCandidate : {t1, t2})
                {
                    if (tCandidate > 0.0f)
                    {
                        float z = localOrigin.z + tCandidate * localDir.z;
                        if (z >= -0.5f && z <= 0.5f)
                        {
                            tSide = tCandidate;
                            break;
                        }
                    }
                }
            }

            // 3. Cap intersection
            float tCap = -1.0f;
            for (float zCap : {-0.5f, 0.5f})
            {
                if (fabs(localDir.z) > 1e-6f)
                {
                    float t = (zCap - localOrigin.z) / localDir.z;
                    if (t > 0.0f)
                    {
                        float x = localOrigin.x + t * localDir.x;
                        float y = localOrigin.y + t * localDir.y;
                        if (x * x + y * y <= 0.25f)
                        {
                            if (tCap < 0.0f || t < tCap)
                                tCap = t;
                        }
                    }
                }
            }

            // 4. Pick the nearest positive t (side or cap)
            float tFinal = -1.0f;
            if (tSide > 0.0f && tCap > 0.0f)
                tFinal = std::min(tSide, tCap);
            else if (tSide > 0.0f)
                tFinal = tSide;
            else if (tCap > 0.0f)
                tFinal = tCap;

            if (tFinal > 0.0f)
            {
                if (outDistance) *outDistance = tFinal;
                return true;
            }
            return false;
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
        glm::quat m_Orientation;
        glm::vec3 m_Scale;
        glm::vec3 m_Color;
        bool m_UseMaterial;

        Material m_Material;

        std::unique_ptr<VertexArray> m_VertexArray;
        std::unique_ptr<VertexBuffer> m_VertexBuffer; // Added member for the vertex buffer
        std::unique_ptr<IndexBuffer> m_IndexBuffer;

        glm::mat4 m_ModelMatrix;

        int m_VertexCount;
        unsigned int m_IndexCount;

        // Generates vertices and indices for the cylinder.
        // Returns the number of vertices (each vertex has 6 floats: position and normal).
        int GenerateCylinder(float baseRadius, float topRadius, float height, int sectorCount,
                             std::vector<float>& vertices, std::vector<unsigned int>& indices);
    };
} // namespace isaacGraphicsEngine
