/**
 *  @file PointLight.h
 *  @brief Header file for the PointLight class.
 *   This class represents a point light source in the scene.
 */

#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Graphics/Primitives/Sphere.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Renderer/Renderer.h"
#include "Utility/config.h"  
#include "Core/IObject.h"

namespace isaacObjectViewer
{
    class PointLight : public IObject
    {
    public:
        /// @brief Constructs a PointLight.
        PointLight(const glm::vec3& position, const glm::vec3& color);

        /// @brief Destroys the PointLight.
        ~PointLight();

        /// @brief Updates the PointLight.
        void Update();

        /// @brief Renders the PointLight.
        /// @param renderer The renderer to use for rendering.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param shader The shader to use for rendering.
        void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader = nullptr) override;

        /// @brief Gets the light's ID.
        /// @return The light's ID.
        std::size_t GetID() const override { return m_ID; }

        /// @brief Gets the light's name.
        /// @return The light's name.
        const std::string& GetName() const override { return m_Name; };

        /// @brief Sets the light's name.
        /// @param newName The new name for the light.
        void SetName(const std::string& newName) override { m_Name = newName; }

        /// @brief Gets the light's type.
        /// @return The light's type.
        ObjectType GetType() const override { return ObjectType::PointLight; }

        /// @brief Gets the light's ambient intensity.
        /// @return The light's ambient intensity.
        glm::vec3& GetAmbientIntensity() { return m_AmbientIntensity; }

        /// @brief Gets the light's diffuse intensity.
        /// @return The light's diffuse intensity.
        glm::vec3& GetDiffuseIntensity() { return m_DiffuseIntensity; }

        /// @brief Gets the light's specular intensity.
        /// @return The light's specular intensity.
        glm::vec3& GetSpecularIntensity() { return m_SpecularIntensity; }

        /// @brief Gets the light's position.
        /// @return The light's position.
        glm::vec3& GetPosition() override { return m_Sphere.GetPosition(); }

        /// @brief Gets the light's rotation.
        /// @return The light's rotation.
        glm::vec3& GetRotation() override 
        { 
            static glm::vec3 zero = glm::vec3(0.0f); 
            return zero; 
        }

        /// @brief Gets the light's orientation.
        /// @return The light's orientation.
        glm::quat& GetOrientation() override 
        { 
            static glm::quat identity = glm::quat(1.0f, 0, 0, 0); 
            return identity;
        }

        /// @brief Gets the light's scale.
        /// @return The light's scale.
        glm::vec3& GetScale() override { return m_Sphere.GetScale(); }

        /// @brief Gets the light's color.
        /// @return The light's color.
        glm::vec3& GetColor() override { return m_Color; }

        /// @brief Gets the light's use material flag.
        /// @return The light's use material flag.
        bool& GetUseMaterial() override { return m_UseMaterial; }

        /// @brief Gets the light's shininess.
        /// @return The light's shininess.
        float& GetShininess() override { return m_Shininess; }

        /// @brief Sets the light's ambient intensity.
        /// @param newAmbientIntensity The new ambient intensity.
        void SetAmbientIntensity(const glm::vec3& newAmbientIntensity) { m_AmbientIntensity = newAmbientIntensity; }

        /// @brief Sets the light's diffuse intensity.
        /// @param newDiffuseIntensity The new diffuse intensity.
        void SetDiffuseIntensity(const glm::vec3& newDiffuseIntensity) { m_DiffuseIntensity = newDiffuseIntensity; }

        /// @brief Sets the light's specular intensity.
        /// @param newSpecularIntensity The new specular intensity.
        void SetSpecularIntensity(const glm::vec3& newSpecularIntensity) { m_SpecularIntensity = newSpecularIntensity; }

        /// @brief Sets the light's position.
        /// @param position The new position.
        void SetPosition(const glm::vec3& position) override { m_Sphere.SetPosition(position); }

        /// @brief Sets the light's rotation.
        /// @param rotation The new rotation.
        void SetRotation(const glm::vec3& rotation) override {}

        /// @brief Sets the light's orientation.
        /// @param newOrientation The new orientation.
        void SetOrientation(const glm::quat& newOrientation) {}

        /// @brief Sets the light's diffuse texture.
        /// @param tex The new diffuse texture.
        void SetDiffuseTexture(const std::shared_ptr<Texture>& tex) override { }

        /// @brief Sets the light's specular texture.
        /// @param tex The new specular texture.
        void SetSpecularTexture(const std::shared_ptr<Texture>& tex) override { }

        /// @brief Sets the light's scale.
        /// @param scale The new scale.
        void SetScale(const glm::vec3& scale) override { m_Sphere.SetScale(scale); }

        /// @brief Sets the light's color.
        /// @param color The new color.
        void SetColor(const glm::vec3& color) override { m_Color = color; }

        /// @brief Sets the light's use material flag.
        /// @param use The new use material flag.
        void SetUseMaterial(bool use) override { m_UseMaterial = use; }

        /// @brief Intersects a ray with the light's volume.
        /// @param ray The ray to intersect.
        /// @param outDistance The distance to the intersection point, if any.
        /// @return True if the ray intersects the light's volume, false otherwise.
        bool IntersectRay(const Ray& ray, float* outDistance) override { return m_Sphere.IntersectRay(ray, outDistance);}

        /// @brief Generates a unique ID for the light.
        /// @return The unique ID.
        std::size_t GenerateUniqueID() override
        {
            static std::size_t currentID = 0;
            return ++currentID;
        }

        /// @brief Gets the light's vertex array.
        /// @return The vertex array.
        inline const VertexArray    &GetVertexArray()   const override { return m_Sphere.GetVertexArray(); }

        /// @brief Gets the light's vertex buffer.
        /// @return The vertex buffer.
        inline const VertexBuffer   &GetVertexBuffer()  const override { return m_Sphere.GetVertexBuffer(); }

        /// @brief Gets the light's index buffer.
        /// @return The index buffer.
        inline const IndexBuffer    &GetIndexBuffer()   const override { return m_Sphere.GetIndexBuffer(); }

        /// @brief Gets the light's index count.
        /// @return The index count.
        unsigned inline int         GetIndexCount()     const override { return m_Sphere.GetIndexCount(); }

        /// @brief Gets the light's vertex count.
        /// @return The vertex count.
        unsigned inline int         GetVertexCount()    const override { return m_Sphere.GetVertexCount(); }
    private:
        std::size_t m_ID;
        std::string m_Name; 
        
        glm::vec3 m_Color;
        bool m_UseMaterial;
        Sphere m_Sphere;
        float m_Shininess = 0.f;

        glm::vec3 m_AmbientIntensity;
        glm::vec3 m_DiffuseIntensity;
        glm::vec3 m_SpecularIntensity;
        
        std::unique_ptr<Shader> m_Shader;
    };
}
