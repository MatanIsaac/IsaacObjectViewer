#include "Light.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Utility/config.h"  
#include "Utility/Log.hpp"

namespace isaacObjectLoader
{
    Light::Light(const glm::vec3& position, const glm::vec3& color)
        : m_Color(color)
        , m_Sphere(position)
        , m_SpecularIntensity(0.5f)
    {
        m_ID = GenerateUniqueID();
        m_Name = "Light_" + std::to_string(m_ID);
        // Scale down the cube representation for the light.
        m_Sphere.SetScale(glm::vec3(0.2f));
        m_Sphere.SetColor(color);

        // Build paths to the light cube shader files.
        std::string lightCubeVS = "src/Resources/Shaders/light_cube.vs";
        std::string lightCubeFS = "src/Resources/Shaders/light_cube.fs";
        ConvertSeparators(lightCubeVS);
        ConvertSeparators(lightCubeFS);

        // Create the shader using a unique pointer.
        m_Shader = std::make_unique<Shader>(lightCubeVS.c_str(), lightCubeFS.c_str());
        if(m_Shader == nullptr)
        {
            LOG_ERROR("Failed to create parse light cube shader!\n");
        }
    }

    Light::~Light()
    {
        // Unique pointer cleans up m_Shader automatically.
    }

    void Light::Update()
    {
        // Update light logic (for example, animate the light position) if needed.
    }

    void Light::Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection)
    {
        // First, update and bind the internal light cube shader.
        m_Shader->Bind();
        m_Shader->setMat4("model", m_Sphere.GetModelMatrix());
        m_Shader->setMat4("view", view);
        m_Shader->setMat4("projection", projection);
        m_Shader->setVec3("lightColor", m_Color);

        // Then update the scene shader with the light properties so that the scene objects
        // can use the light’s color, position, and specular intensity.
        shader.Bind();
        shader.setVec3("lightColor", m_Color);
        shader.setVec3("lightPos", m_Sphere.GetPosition());
        shader.setFloat("specularIntensity", m_SpecularIntensity);

        // Render the light cube using indexed drawing.
        renderer.Render(m_Sphere.GetVertexArray(), m_Sphere.GetIndexBuffer(), *m_Shader);
    }
}
