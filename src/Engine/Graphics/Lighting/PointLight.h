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
#include "Graphics/Material.h"
#include "Core/IObject.h"

namespace isaacObjectViewer
{
    class PointLight : public IObject
    {
    public:
        /// @brief Constructs a PointLight.
        PointLight(const glm::vec3& position, const glm::vec3& color);

        /// @brief Destroys the PointLight.
        ~PointLight() = default;

        /// @brief Updates the PointLight.
        void Update();

        /// @brief Enables the light.
        void Enable() { m_Enabled = true; }

        /// @brief Disables the light.
        void Disable() { m_Enabled = false; }

        /// @brief Checks if the light is enabled.
        /// @return True if the light is enabled, false otherwise.
        bool IsEnabled() const { return m_Enabled; }

        /// @brief Sets the light's enabled state.
        /// @param enabled The new enabled state.
        void SetEnabled(bool enabled) { m_Enabled = enabled; }

        /// @brief Renders the PointLight.
        /// @param renderer The renderer to use for rendering.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param shader The shader to use for rendering.
        void Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, std::unique_ptr<Shader>& shader) override;

        /// @brief Gets the light's ID.
        /// @return The light's ID.
        const std::size_t& GetID() const override { return m_ID; }

        /// @brief Gets the light's name.
        /// @return The light's name.
        const std::string& GetName() const override { return m_Name; };

        /// @brief Sets the light's name.
        /// @param newName The new name for the light.
        void SetName(const std::string& newName) override { m_Name = newName; }

        /// @brief Gets the light's type.
        /// @return The light's type.
        const ObjectType& GetType() const override { return m_Type; }

        /// @brief Gets the light's ambient intensity.
        /// @return The light's ambient intensity.
        const glm::vec3& GetAmbientIntensity() const { return m_AmbientIntensity; }

        /// @brief Gets the light's diffuse intensity.
        /// @return The light's diffuse intensity.
        const glm::vec3& GetDiffuseIntensity() const { return m_DiffuseIntensity; }

        /// @brief Gets the light's specular intensity.
        /// @return The light's specular intensity.
        const glm::vec3& GetSpecularIntensity() const { return m_SpecularIntensity; }

        /// @brief Gets the light's attenuation constant.
        /// @return The light's attenuation constant.
        const float& GetAttConstant() const { return m_AttConstant; }

        /// @brief Gets the light's attenuation linear.
        /// @return The light's attenuation linear.
        const float& GetAttLinear() const { return m_AttLinear; }

        /// @brief Gets the light's attenuation quadratic.
        /// @return The light's attenuation quadratic.
        const float& GetAttQuadratic() const { return m_AttQuadratic; }

        /// @brief Gets the light's position.
        /// @return The light's position.
        const glm::vec3& GetPosition() const override { return m_Sphere.GetPosition(); }

        /// @brief Gets the light's rotation.
        /// @return The light's rotation.
        const glm::vec3& GetRotation() const override 
        { 
            static glm::vec3 zero = glm::vec3(0.0f); 
            return zero; 
        }

        /// @brief Gets the light's orientation.
        /// @return The light's orientation.
        const glm::quat& GetOrientation() const override 
        { 
            static glm::quat identity = glm::quat(1.0f, 0, 0, 0); 
            return identity;
        }

        /// @brief Gets the light's scale.
        /// @return The light's scale.
        const glm::vec3& GetScale() const override { return m_Sphere.GetScale(); }

        /// @brief Gets the light's color.
        /// @return The light's color.
        const glm::vec3& GetColor() const override { return m_Color; }

        /// @brief Gets the light's material.
        /// @return The light's material.
        const Material& GetMaterial() const override { return m_Sphere.GetMaterial(); }

        /// @brief Gets the light's use material flag.
        /// @return The light's use material flag.
        const bool& GetUseMaterial() const override { return m_UseMaterial; }

        /// @brief Gets the light's shininess.
        /// @return The light's shininess.
        const float& GetShininess() const override { return m_Shininess; }

        /// @brief Sets the light's ambient intensity.
        /// @param newAmbientIntensity The new ambient intensity.
        void SetAmbientIntensity(const glm::vec3& newAmbientIntensity) { m_AmbientIntensity = newAmbientIntensity; }

        /// @brief Sets the light's diffuse intensity.
        /// @param newDiffuseIntensity The new diffuse intensity.
        void SetDiffuseIntensity(const glm::vec3& newDiffuseIntensity) { m_DiffuseIntensity = newDiffuseIntensity; }

        /// @brief Sets the light's specular intensity.
        /// @param newSpecularIntensity The new specular intensity.
        void SetSpecularIntensity(const glm::vec3& newSpecularIntensity) { m_SpecularIntensity = newSpecularIntensity; }

        /// @brief Sets the light's attenuation constant.
        /// @param newAttConstant The new attenuation constant.
        void SetAttConstant(float newAttConstant) { m_AttConstant = newAttConstant; }

        /// @brief Sets the light's attenuation linear.
        /// @param newAttLinear The new attenuation linear.
        void SetAttLinear(float newAttLinear) { m_AttLinear = newAttLinear; }

        /// @brief Sets the light's attenuation quadratic.
        /// @param newAttQuadratic The new attenuation quadratic.
        void SetAttQuadratic(float newAttQuadratic) { m_AttQuadratic = newAttQuadratic; }

        /// @brief Sets the light's position.
        /// @param position The new position.
        void SetPosition(const glm::vec3& position) override { m_Sphere.SetPosition(position); }

        /// @brief Sets the rotation of the point light.
        /// @param newRotation The new rotation of the point light.
        virtual void SetRotation(const glm::vec3& newRotation) override { m_Sphere.SetRotation(newRotation); }

        /// @brief Sets the orientation of the point light.
        /// @param newOrientation The new orientation of the point light.
        virtual void SetOrientation(const glm::quat& newOrientation) override { m_Sphere.SetOrientation(newOrientation); }

        /// @brief Sets the light's scale.
        /// @param scale The new scale.
        void SetScale(const glm::vec3& scale) override { m_Sphere.SetScale(scale); }

        /// @brief Sets the light's color.
        /// @param color The new color.
        void SetColor(const glm::vec3& color) override { m_Color = color; }

        /// @brief Sets the light's material.
        /// @param material The new material.
        void SetMaterial(const Material& material) override { m_Sphere.SetMaterial(material); }
        
        /// @brief Sets the light's use material flag.
        /// @param use The new use material flag.
        void SetUseMaterial(bool use) override { m_UseMaterial = use; }

        /// @brief Sets the light's shininess.
        /// @param newShininess The new shininess.
        void SetShininess(float newShininess) override { m_Material.Shininess = newShininess; }

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

        /// @brief Sets the uniforms for this point light in the given shader.
        /// @param shader The shader to set the uniforms for.
        /// @param lightIndex The index of this light in the uniform array.
        void SetLightUniforms(std::unique_ptr<Shader>& shader, const std::string& uniformName) const;
        
    private:
        std::size_t m_ID;
        std::string m_Name; 
        ObjectType m_Type;
        glm::vec3 m_Color;
        bool m_Enabled;
        bool m_UseMaterial;
        Material m_Material;
        Sphere m_Sphere;
        float m_Shininess = 0.f;

        glm::vec3 m_AmbientIntensity;
        glm::vec3 m_DiffuseIntensity;
        glm::vec3 m_SpecularIntensity;

        float m_AttConstant;
        float m_AttLinear;
        float m_AttQuadratic;

        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<Shader> m_UnlitShader;
    };
}
