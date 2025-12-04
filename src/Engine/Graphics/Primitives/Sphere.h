/**
 * @file Sphere.h
 * @brief Header file of the Sphere class.
 * This class represents a sphere primitive in the scene.
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
#include "Core/IObject.h"
#include "Texture.h"
#include "Material.h"


namespace isaacObjectViewer
{
    class Sphere : public IObject
    {
    public:
        /// @brief Constructs a sphere at the given position.
        /// @param position The position of the sphere.
        Sphere(const glm::vec3& position = DEFAULT_POSITION);
        
        /// @brief Destroys the sphere.
        ~Sphere() override;

        /// @brief Updates the sphere's state.
        void Update();
        
        /// @brief Renders the sphere.
        /// @param renderer The renderer to use.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param shader The shader to use (optional).
        void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, std::unique_ptr<Shader>& shader) override;
        
        /// @brief  Gets the ID of the sphere.
        /// @return The ID of the sphere.
        const std::size_t& GetID() const override{ return m_ID; }
        
        /// @brief Gets the name of the sphere.
        /// @return The name of the sphere.
        const std::string& GetName() const override { return m_Name; };
        
        /// @brief Gets the type of the sphere.
        /// @return The type of the sphere.
        const ObjectType& GetType() const override{ return m_Type; }
        
        /// @brief Gets the position of the sphere.
        /// @return The position of the sphere.
        const glm::vec3& GetPosition() const override { return m_Position; }
        
        /// @brief Gets the rotation of the sphere.
        /// @return The rotation of the sphere.
        const glm::vec3& GetRotation() const override { return m_Rotation; }

        /// @brief Gets the orientation of the sphere.
        /// @return The orientation of the sphere.
        const glm::quat& GetOrientation() const override { return m_Orientation; }

        /// @brief Gets the scale of the sphere.
        /// @return The scale of the sphere.
        const glm::vec3& GetScale() const override { return m_Scale; }

        /// @brief Gets the color of the sphere.
        /// @return The color of the sphere.
        const glm::vec3& GetColor() const override { return m_Color; }

        /// @brief Gets the material of the sphere.
        /// @return The material of the sphere.
        const Material& GetMaterial() const override { return m_Material; }

        /// @brief Gets whether the sphere uses a material.
        /// @return True if the sphere uses a material, false otherwise.
        const bool& GetUseMaterial() const override { return m_UseMaterial; }

        /// @brief Gets the shininess of the sphere's material.
        /// @return The shininess of the sphere's material.
        const float& GetShininess() const override { return m_Material.Shininess; }

        /// @brief Resets the position of the sphere to the default.
        void ResetPosition() { m_Position = DEFAULT_POSITION; }
        
        /// @brief Resets the rotation of the sphere to the default.
        void ResetRotation() { m_Rotation = DEFAULT_ROTATION; }
        
        /// @brief Resets the scale of the sphere to the default.
        void ResetScale() { m_Scale = DEFAULT_SCALE; }
        
        /// @brief Sets the name of the sphere.
        /// @param newName The new name for the sphere.
        void SetName(const std::string& newName) override { m_Name = newName; }
        
        /// @brief Sets the position of the sphere.
        /// @param newPosition The new position for the sphere.
        inline void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }
        
        /// @brief Sets the rotation of the sphere.
        /// @param newRotation The new rotation for the sphere.
        inline void SetRotation(const glm::vec3& newRotation) override { m_Rotation = newRotation; }
        
        /// @brief Sets the orientation of the sphere.
        /// @param newOrientation The new orientation for the sphere.
        inline void SetOrientation(const glm::quat& newOrientation) override { m_Orientation = newOrientation; }
        
        /// @brief Sets the scale of the sphere.
        /// @param newScale The new scale for the sphere.
        inline void SetScale(const glm::vec3& newScale) override { m_Scale = newScale; }
        
        /// @brief Sets the color of the sphere.
        /// @param newColor The new color for the sphere.
        inline void SetColor(const glm::vec3& newColor) override { m_Color = newColor; }
        
        /// @brief Sets the material of the sphere.
        /// @param newMaterial The new material for the sphere.
        void SetMaterial(const Material& newMaterial) override { m_Material = newMaterial; }
        
        /// @brief Sets whether the sphere uses a material.
        /// @param useMaterial True if the sphere should use a material, false otherwise.
        inline void SetUseMaterial(bool useMaterial) override { m_UseMaterial = useMaterial; }

        /// @brief Sets the shininess of the sphere's material.
        /// @param newShininess The new shininess value for the sphere's material.
        void SetShininess(float newShininess) override { m_Material.Shininess = newShininess; }

        /// @brief Checks for intersection with a ray.
        /// @param ray The ray to check for intersection.
        /// @param outDistance The distance to the intersection point, if one exists.
        /// @return True if the ray intersects the sphere, false otherwise.
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

        /// @brief Generates a unique ID for the object.
        /// @return The unique ID.
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

        std::unique_ptr<VertexArray> m_VertexArray;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;

        int m_VertexCount;
        unsigned int m_IndexCount;

        
        /// @brief Generates vertices and indices for a sphere, using a radius,
        /// sectorCount (longitude divisions) and stackCount (latitude divisions).
        /// Each vertex contains 6 floats (position and normal).
        /// @param radius The radius of the sphere.
        /// @param sectorCount The number of vertical sectors.
        /// @param stackCount The number of horizontal stacks.
        /// @param vertices The output vertex data.
        /// @param indices The output index data.
        /// @return 
        int GenerateSphere(float radius, int sectorCount, int stackCount,
                           std::vector<float>& vertices, std::vector<unsigned int>& indices);
    };
} // namespace isaacGraphicsEngine
