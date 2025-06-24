#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Engine/Graphics/OpenGL/Primitives/Cube.h"
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
        glm::vec3& GetPosition() { return m_Cube.GetPosition(); }
        glm::vec3& GetColor() { return m_Color; }

        void SetSpecularIntensity(float newSpecularIntensity) { m_SpecularIntensity = newSpecularIntensity; }
        void SetPosition(const glm::vec3& position) { m_Cube.SetPosition(position); }
        void SetColor(const glm::vec3& color) { m_Color = color; }
        Cube* GetCube() { return &m_Cube; }
    private:
        glm::vec3 m_Color;
        Cube m_Cube;
        float m_SpecularIntensity;
        std::unique_ptr<Shader> m_Shader;
    };
}
