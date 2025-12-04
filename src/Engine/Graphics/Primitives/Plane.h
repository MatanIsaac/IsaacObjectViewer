/**
 * @file Plane.h
 * @brief Header file for the Plane class.
 * This class represents a plane primitive in the scene.
 */

#pragma once

#include "Utility/config.h"
#include <memory>
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Renderer/Renderer.h"
#include "IObject.h"
#include "Texture.h"
#include "Material.h"

namespace isaacObjectViewer
{
    class Plane : public IObject
    {
    public:
        /// @brief Constructs a Plane.
        /// @param position The position of the plane.
        Plane(const glm::vec3& position = DEFAULT_POSITION);

        /// @brief Destroys the Plane.
        ~Plane() override;

        /// @brief Updates the Plane.
        void Update();

        /// @brief Renders the Plane.
        /// @param renderer The renderer to use.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param shader The shader to use.
        void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, std::unique_ptr<Shader>& shader) override;

        /// @brief Gets the ID of the Plane.
        /// @return The ID of the Plane.
        const std::size_t& GetID() const override { return m_ID; }

        /// @brief Gets the name of the Plane.
        /// @return The name of the Plane.
        const std::string& GetName() const override { return m_Name; };

        /// @brief Gets the type of the Plane.
        /// @return The type of the Plane.
        const ObjectType& GetType() const { return m_Type; }

        /// @brief Gets the position of the Plane.
        /// @return The position of the Plane.
        const glm::vec3& GetPosition() const override { return m_Position; }

        /// @brief Gets the rotation of the Plane.
        /// @return The rotation of the Plane.
        const glm::vec3& GetRotation() const override { return m_Rotation; }

        /// @brief Gets the orientation of the Plane.
        /// @return The orientation of the Plane.
        const glm::quat& GetOrientation() const override { return m_Orientation; }

        /// @brief Gets the scale of the Plane.
        /// @return The scale of the Plane.
        const glm::vec3& GetScale() const override { return m_Scale; }

        /// @brief Gets the color of the Plane.
        /// @return The color of the Plane.
        const glm::vec3& GetColor() const { return m_Color; }

        /// @brief Gets the material of the Plane.
        /// @return The material of the Plane.
        const Material& GetMaterial() const override { return m_Material; }

        /// @brief Gets whether the Plane uses a material.
        /// @return True if the Plane uses a material, false otherwise.
        const bool& GetUseMaterial() const override { return m_UseMaterial; }

        /// @brief Gets the shininess of the material.
        /// @return The shininess of the material.
        const float& GetShininess() const override { return m_Material.Shininess; }

        /// @brief Resets the position of the Plane to the default.
        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        
        /// @brief Resets the rotation of the Plane to the default.
        void ResetRotation() { m_Rotation = DEFAULT_ROTATION; }
        
        /// @brief Resets the scale of the Plane to the default.
        void ResetScale() { m_Scale = DEFAULT_SCALE; }
        
        /// @brief Sets the name of the Plane.
        /// @param newName The new name of the Plane.
        void SetName(const std::string& newName) override { m_Name = newName; }

        /// @brief Sets the position of the Plane.
        /// @param newPosition The new position of the Plane.
        inline void SetPosition(const glm::vec3& newPosition) override          { m_Position = newPosition; }

        /// @brief Sets the rotation of the Plane.
        /// @param rotation The new rotation of the Plane.
        inline void SetRotation(const glm::vec3& rotation) override             { m_Rotation = rotation; }

        /// @brief Sets the orientation of the Plane.
        /// @param newOrientation The new orientation of the Plane.
        inline void SetOrientation(const glm::quat& newOrientation) override    { m_Orientation = newOrientation; }

        /// @brief Sets the scale of the Plane.
        /// @param scale The new scale of the Plane.
        inline void SetScale(const glm::vec3& scale) override                   { m_Scale = scale; }

        /// @brief Sets the color of the Plane.
        /// @param newColor The new color of the Plane.
        void SetColor(const glm::vec3& newColor) { m_Color = newColor; }

        /// @brief Sets the material of the Plane.
        /// @param newMaterial The new material of the Plane.
        void SetMaterial(const Material& newMaterial) override { m_Material = newMaterial; }

        /// @brief Sets whether the Plane uses a material.
        /// @param useMaterial True if the Plane should use a material, false otherwise.
        void SetUseMaterial(bool useMaterial) override { m_UseMaterial = useMaterial; }

        /// @brief Sets the shininess of the material.
        /// @param newShininess The new shininess value for the material.
        void SetShininess(float newShininess) override { m_Material.Shininess = newShininess; }

        /// @brief Checks for intersection with a ray.
        /// @param ray The ray to check for intersection.
        /// @param outDistance The distance to the intersection point, if one exists.
        /// @return True if the ray intersects the Plane, false otherwise.
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

        /// @brief Generates a unique ID for the Plane.
        /// @return The unique ID for the Plane.
        inline std::size_t GenerateUniqueID() override
        {
            static std::size_t currentID = 0;
            return ++currentID;
        }

    private:
        std::size_t m_ID;
        std::string m_Name;
        ObjectType m_Type;
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
        int m_VertexCount;
        static constexpr int m_FloatsPerVertex = 8;
        static constexpr int m_IndicesCount = 6;
    public:
        /// @brief Gets the vertices of the Plane.
        static constexpr float m_PlaneVertices[] = 
        { 
            /**
             * positions          normals             TexCoords     Tangents    Bitangents
            */
            -0.5f, -0.5f,   0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   1,0,0,      0,1,0, // bottom-left
             0.5f, -0.5f,   0.0f, 0.0f, 0.0f, 1.0f,   1.0f, 0.0f,   1,0,0,      0,1,0, // bottom-right
             0.5f,  0.5f,   0.0f, 0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   1,0,0,      0,1,0, // top-right
            -0.5f,  0.5f,   0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f,   1,0,0,      0,1,0, // top-left
        };

        /// @brief Gets the indices of the Plane.
        static constexpr unsigned int m_PlaneIndices[m_IndicesCount] =
        {
            0, 1, 2,
            2, 3, 0
        };
    };
}
