#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Graphics/Shaders/Shader.h"
#include "Utility/config.h"  

namespace isaacObjectViewer
{
    class DirectionalLight
    {
    public:
        DirectionalLight();
        ~DirectionalLight();

        void Update();
        void SetUniforms(Shader* shader);
        glm::vec3 GetDirection() { return m_Direction; } 
        glm::vec3 GetAmbient() { return m_Ambient; } 
        glm::vec3 GetDiffuse() { return m_Diffuse; } 
        glm::vec3 GetSpecular() { return m_Specular; } 

        void SetDirection(const glm::vec3& newDirection) 
        { 
            m_Direction = newDirection; 
            m_Changed = true;
        } 
        void SetAmbient(const glm::vec3& newAmbient) 
        { 
            m_Ambient = newAmbient; 
            m_Changed = true;
        } 
        void SetDiffuse(const glm::vec3& newDiffuse) 
        { 
            m_Diffuse = newDiffuse; 
            m_Changed = true;
        } 
        void SetSpecular(const glm::vec3& newSpecular) 
        { 
            m_Specular = newSpecular; 
            m_Changed = true;
        } 
    
    private:
        glm::vec3 m_Direction;
        glm::vec3 m_Ambient;
        glm::vec3 m_Diffuse;
        glm::vec3 m_Specular;
        bool m_Changed;
    };
}
