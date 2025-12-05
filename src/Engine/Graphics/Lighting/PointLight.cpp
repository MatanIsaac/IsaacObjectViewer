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
        std::string pointlightVS = GetProjectRootPath("src/Resources/Shaders/pointlight.vs");
        std::string pointlightFS = GetProjectRootPath("src/Resources/Shaders/pointlight.fs");
        
        // Create the shader using a unique pointer.
        m_Shader = std::make_unique<Shader>(pointlightVS.c_str(), pointlightFS.c_str());
        if(m_Shader == nullptr)
        {
            LOG_ERROR("Failed to create parse point light shader!\n");
        }

        if(!m_Sphere.IsUnlit() && !m_Sphere.EnableUnlit("src/Resources/Shaders/pointlight.vs",GetProjectRootPath("src/Resources/Shaders/unlit.fs")))
        {
            LOG_ERROR("Failed to enable unlit shader for PointLight sphere.");
        }
    }

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

        if(IsUnlit())
            m_Sphere.Render(renderer, view, projection, GetUnlitShader());
        else
            m_Sphere.Render(renderer, view, projection, m_Shader);
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