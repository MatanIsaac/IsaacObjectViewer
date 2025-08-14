/**
 *  @file DirectionalLight.h
 *  @brief Header file for the DirectionalLight class.
 *   This class represents a directional light source in the scene.
 */

#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Graphics/Shader/Shader.h"
#include "Utility/config.h"  

namespace isaacObjectViewer
{
    class DirectionalLight
    {
    public:

        /// @brief Constructs a DirectionalLight.
        DirectionalLight();

        /// @brief Destroys the DirectionalLight.
        ~DirectionalLight();

        /// @brief Updates the light's properties.
        void Update();

        /// @brief Sets the light's uniforms in the shader.
        /// @param shader The shader to set the uniforms for.
        void SetUniforms(Shader* shader);

        /// @brief Gets the light's direction.
        /// @return The light's direction.
        glm::vec3 GetDirection() { return m_Direction; } 
        
        /// @return The light's ambient color.
        glm::vec3 GetAmbient() { return m_Ambient; } 
        
        /// @brief  Gets the light's diffuse color.
        /// @return The light's diffuse color.
        glm::vec3 GetDiffuse() { return m_Diffuse; } 

        /// @brief  Gets the light's specular color.
        /// @return The light's specular color.
        glm::vec3 GetSpecular() { return m_Specular; } 

        /// @brief Sets the light's direction.
        /// @param newDirection The new direction for the light.
        void SetDirection(const glm::vec3& newDirection) 
        { 
            m_Direction = newDirection; 
            m_Changed = true;
        }

        /// @brief Sets the light's ambient color.
        /// @param newAmbient The new ambient color for the light.
        void SetAmbient(const glm::vec3& newAmbient) 
        { 
            m_Ambient = newAmbient; 
            m_Changed = true;
        } 

        /// @brief Sets the light's diffuse color.
        /// @param newDiffuse The new diffuse color for the light.
        void SetDiffuse(const glm::vec3& newDiffuse) 
        { 
            m_Diffuse = newDiffuse; 
            m_Changed = true;
        } 

        /// @brief  Sets the light's specular color.
        /// @param newSpecular The new specular color for the light.
        void SetSpecular(const glm::vec3& newSpecular) 
        { 
            m_Specular = newSpecular; 
            m_Changed = true;
        } 
    
    private:
        glm::vec3 m_Direction; ///< The light's direction.
        glm::vec3 m_Ambient; ///< The light's ambient color.
        glm::vec3 m_Diffuse; ///< The light's diffuse color.
        glm::vec3 m_Specular; ///< The light's specular color.
        bool m_Changed; ///< Whether the light's properties have changed.
    };
}
