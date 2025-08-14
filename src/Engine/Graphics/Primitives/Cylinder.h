/**
 *  @file Cylinder.h
 *  @brief Header file for the Cylinder class.
 *   This class represents a cylinder primitive in the scene.
 */

#pragma once

#include "Utility/config.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Renderer/Renderer.h"
#include "IObject.h"
#include "Material.h"

namespace isaacObjectViewer
{
    class Cylinder : public IObject
    {
    public:
        /// @brief Constructs a Cylinder.
        /// @param position The position of the cylinder.
        Cylinder(const glm::vec3& position = DEFAULT_POSITION);

        /// @brief Destroys the Cylinder.
        ~Cylinder() override;

        /// @brief Updates the Cylinder.
        void Update();

        /// @brief Renders the Cylinder.
        /// @param renderer The renderer to use for rendering.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param shader The shader to use for rendering.
        void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader = nullptr) override;

        /// @brief Gets the cylinder's ID.
        /// @return The cylinder's ID.
        std::size_t GetID() const { return m_ID; }

        /// @brief Gets the cylinder's name.
        /// @return The cylinder's name.
        const std::string& GetName() const override { return m_Name; };

        /// @brief Sets the cylinder's name.
        /// @param newName The new name for the cylinder.
        void SetName(const std::string& newName) override { m_Name = newName; }

        /// @brief Gets the cylinder's type.
        /// @return The cylinder's type.
        ObjectType GetType() const { return ObjectType::Cylinder; }

        /// @brief Gets the cylinder's position.
        /// @return The cylinder's position.
        glm::vec3& GetPosition() override { return m_Position; }

        /// @brief Gets the cylinder's rotation.
        /// @return The cylinder's rotation.
        glm::vec3& GetRotation() override { return m_Rotation; }

        /// @brief Gets the cylinder's orientation.
        /// @return The cylinder's orientation.
        glm::quat& GetOrientation() override { return m_Orientation; }

        /// @brief Gets the cylinder's scale.
        /// @return The cylinder's scale.
        glm::vec3& GetScale() override { return m_Scale; }

        /// @brief Gets the cylinder's color.
        /// @return The cylinder's color.
        glm::vec3& GetColor() override { return m_Color; }

        /// @brief Gets whether the cylinder uses a material.
        /// @return True if the cylinder uses a material, false otherwise.
        bool& GetUseMaterial() override { return m_UseMaterial; }

        /// @brief Gets the cylinder's shininess.
        /// @return The cylinder's shininess.
        float& GetShininess() override { return m_Material.Shininess; }

        /// @brief Gets the cylinder's material.
        /// @return The cylinder's material.
        const Material& GetMaterial() const { return m_Material; }

        /// @brief Resets the cylinder's position to the default.
        void ResetPosition() { m_Position = DEFAULT_POSITION; }

        /// @brief Resets the cylinder's rotation to the default.
        void ResetRotation() { m_Rotation = DEFAULT_ROTATION; }

        /// @brief Resets the cylinder's scale to the default.
        void ResetScale() { m_Scale = DEFAULT_SCALE; }

        /// @brief Sets the cylinder's position.
        /// @param newPosition The new position for the cylinder.
        inline void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }

        /// @brief Sets the cylinder's rotation.
        /// @param newRotation The new rotation for the cylinder.
        inline void SetRotation(const glm::vec3& newRotation) override { m_Rotation = newRotation; }

        /// @brief Sets the cylinder's orientation.
        /// @param newOrientation The new orientation for the cylinder.
        inline void SetOrientation(const glm::quat& newOrientation) override { m_Orientation = newOrientation; }

        /// @brief Sets the cylinder's scale.
        /// @param newScale The new scale for the cylinder.
        inline void SetScale(const glm::vec3& newScale) override { m_Scale = newScale; }

        /// @brief Sets the cylinder's color.
        /// @param newColor The new color for the cylinder.
        inline void SetColor(const glm::vec3& newColor) override { m_Color = newColor; }

        /// @brief Sets whether the cylinder uses a material.
        /// @param useMaterial True if the cylinder should use a material, false otherwise.
        inline void SetUseMaterial(bool useMaterial) override { m_UseMaterial = useMaterial; }

        /// @brief Sets the cylinder's material.
        /// @param newMaterial The new material for the cylinder.
        void SetMaterial(const Material& newMaterial) { m_Material = newMaterial; }

        /// @brief Sets the cylinder's diffuse texture.
        /// @param tex The new diffuse texture for the cylinder.
        void SetDiffuseTexture(const std::shared_ptr<Texture>& tex) override { }

        /// @brief Sets the cylinder's specular texture.
        /// @param tex The new specular texture for the cylinder.
        void SetSpecularTexture(const std::shared_ptr<Texture>& tex) override { }

        /// @brief Gets the cylinder's vertex array.
        /// @return The cylinder's vertex array.
        inline const VertexArray    &GetVertexArray()   const override { return *m_VertexArray; }

        /// @brief Gets the cylinder's vertex buffer.
        /// @return The cylinder's vertex buffer.
        inline const VertexBuffer   &GetVertexBuffer()  const override { return *m_VertexBuffer; }

        /// @brief Gets the cylinder's index buffer.
        /// @return The cylinder's index buffer.
        inline const IndexBuffer    &GetIndexBuffer()   const override { return *m_IndexBuffer; }

        /// @brief Gets the cylinder's index count.
        /// @return The cylinder's index count.
        unsigned inline int         GetIndexCount()     const override { return m_IndexCount; }

        /// @brief Gets the cylinder's vertex count.
        /// @return The cylinder's vertex count.
        inline unsigned int GetVertexCount() const override{ return m_VertexCount; }

        /// @brief Checks for intersection between a ray and the cylinder.
        /// @param ray The ray to check for intersection.
        /// @param outDistance The distance to the intersection point, if one exists.
        /// @return True if the ray intersects the cylinder, false otherwise.
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

        /// @brief Generates a unique ID for the cylinder.
        /// @return The unique ID.
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
        
        /// @brief Generates vertices and indices for the cylinder.
        /// Returns the number of vertices (each vertex has 6 floats: position and normal).
        /// @param baseRadius The radius of the base of the cylinder.
        /// @param topRadius The radius of the top of the cylinder.
        /// @param height The height of the cylinder.
        /// @param sectorCount The number of sectors (triangles) to use for the cylinder.
        /// @param vertices The output vertex data.
        /// @param indices The output index data.
        /// @return The number of vertices generated.
        int GenerateCylinder(float baseRadius, float topRadius, float height, int sectorCount,
                             std::vector<float>& vertices, std::vector<unsigned int>& indices);
    };
} // namespace isaacObjectViewer
