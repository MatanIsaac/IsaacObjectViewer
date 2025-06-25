#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Engine/Graphics/OpenGL/Primitives/Sphere.h"
#include "Engine/Graphics/OpenGL/Shaders/Shader.h"
#include "Engine/Graphics/OpenGL/Renderer/Renderer.h"
#include "Utility/config.h"  // for GetProjectRoot()

namespace isaacObjectLoader
{
    class Light
    {
    public:
        Light(const glm::vec3& position, const glm::vec3& color);
        ~Light();

        void Update();
        // The 'sceneShader' parameter is the shader used to render scene objects,
        // which we update with light properties.
        void Render(const Renderer& renderer, Shader& sceneShader, const glm::mat4& view, const glm::mat4& projection);

        float& GetSpecularIntensity() { return m_SpecularIntensity; }
        glm::vec3& GetPosition() { return m_Sphere.GetPosition(); }
        glm::vec3& GetRotation() { return m_Sphere.GetRotation(); }
        glm::vec3& GetScale() { return m_Sphere.GetScale(); }
        glm::vec3& GetColor() { return m_Color; }

        void SetSpecularIntensity(float newSpecularIntensity) { m_SpecularIntensity = newSpecularIntensity; }
        void SetPosition(const glm::vec3& position) { m_Sphere.SetPosition(position); }
        void SetRotation(const glm::vec3& rotation) { m_Sphere.SetRotation(rotation); }
        void SetScale(const glm::vec3& scale) { m_Sphere.SetScale(scale); }
        void SetColor(const glm::vec3& color) { m_Color = color; }
        ISceneObject* GetSceneObject() { return &m_Sphere; }
    private:
        glm::vec3 m_Color;
        Sphere m_Sphere;
        float m_SpecularIntensity;
        std::unique_ptr<Shader> m_Shader;
    };
}
