#include "PointLight.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Utility/config.h"  
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{
    PointLight::PointLight(const glm::vec3& position, const glm::vec3& color)
        : m_ID(GenerateUniqueID())
        , m_Name("Light_" + std::to_string(m_ID))
        , m_Type(ObjectType::PointLight)
        , m_Color(color)
        , m_Enabled(true)
        , m_UseMaterial(false)
        , m_Sphere(position)
        , m_AmbientIntensity(0.03f)
        , m_DiffuseIntensity(1.0f)
        , m_SpecularIntensity(1.0f)
        , m_AttConstant(1.0f)
        , m_AttLinear(0.09f)
        , m_AttQuadratic(0.032f)
    {
        
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

    void PointLight::Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, std::unique_ptr<Shader>& shader)
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

        m_Sphere.Render(renderer, view, projection, shader);
    }
    
    void PointLight::SetLightUniforms(std::unique_ptr<Shader>& shader, const std::string& uniformName) const
    {
        shader->setBool(uniformName + ".enabled", m_Enabled);
        shader->setVec3(uniformName + ".position", GetPosition());
        shader->setVec3(uniformName + ".ambient", m_AmbientIntensity);
        shader->setVec3(uniformName + ".diffuse", m_DiffuseIntensity);
        shader->setVec3(uniformName + ".specular", m_SpecularIntensity);
        shader->setFloat(uniformName + ".att_constant", m_AttConstant);
        shader->setFloat(uniformName + ".att_linear", m_AttLinear);
        shader->setFloat(uniformName + ".att_quadratic", m_AttQuadratic);
    }
}