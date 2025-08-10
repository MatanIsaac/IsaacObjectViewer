#include "PointLight.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Utility/config.h"  
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{
    PointLight::PointLight(const glm::vec3& position, const glm::vec3& color)
        : m_Color(color)
        , m_Sphere(position)
        , m_AmbientIntensity({0.2f, 0.2f, 0.2f})
        , m_DiffuseIntensity({0.4f, 0.4f, 0.4f})
        , m_SpecularIntensity({0.5f, 0.5f, 0.5f})
    {
        m_ID = GenerateUniqueID();
        m_Name = "Light_" + std::to_string(m_ID);
        // Scale down the cube representation for the light.
        m_Sphere.SetScale(glm::vec3(0.2f));
        m_Sphere.SetColor(color);

        // Build paths to the light cube shader files.
        std::string lightCubeVS = GetProjectRootPath("src/Resources/Shaders/light_cube.vs");
        std::string lightCubeFS = GetProjectRootPath("src/Resources/Shaders/light_cube.fs");
        
        GetProjectRootPath(lightCubeFS);

        // Create the shader using a unique pointer.
        m_Shader = std::make_unique<Shader>(lightCubeVS.c_str(), lightCubeFS.c_str());
        if(m_Shader == nullptr)
        {
            LOG_ERROR("Failed to create parse light cube shader!\n");
        }
    }

    PointLight::~PointLight()
    { }

    void PointLight::Update()
    { }

    void PointLight::Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader)
    {
        if(shader == nullptr)
        {
            LOG_INFO("Shader parameter is null!");
            return;
        }
        // update and bind the internal light cube shader.
        m_Shader->Bind();
        m_Shader->setMat4("model", m_Sphere.GetModelMatrix());
        m_Shader->setMat4("view", view);
        m_Shader->setMat4("projection", projection);
        m_Shader->setVec3("lightColor", m_Color);

        // Render the light cube using indexed drawing.
        renderer.Render(m_Sphere.GetVertexArray(), m_Sphere.GetIndexBuffer(), *m_Shader);
    }
}
