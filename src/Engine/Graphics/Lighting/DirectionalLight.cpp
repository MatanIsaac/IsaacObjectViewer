#include "DirectionalLight.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Utility/config.h"  
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{
    DirectionalLight::DirectionalLight()
        : m_Direction({-0.2f, -0.3f, -0.3f})
        , m_Ambient({0.8f, 0.8f, 0.8f})
        , m_Diffuse({0.7f, 0.7f, 0.7f})
        , m_Specular({0.6f, 0.6f, 0.6f})
        , m_Changed(true)
    {}

    DirectionalLight::~DirectionalLight()
    {
    }

    void DirectionalLight::Update()
    { }

    void DirectionalLight::SetUniforms(Shader* shader)
    {
        if(shader == nullptr)
        {
            LOG_INFO("Shader parameter is null!");
            return;
        }
        if(m_Changed)
        {
            shader->setVec3("dirLight.direction", m_Direction);
            shader->setVec3("dirLight.ambient", m_Ambient);
            shader->setVec3("dirLight.diffuse", m_Diffuse);
            shader->setVec3("dirLight.specular", m_Specular);
            m_Changed = false;
        }
    }
}
