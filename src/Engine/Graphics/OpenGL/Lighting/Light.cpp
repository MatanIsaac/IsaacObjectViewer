#include "Light.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace isaacGraphicsEngine
{
    Light::Light(const glm::vec3& position, const glm::vec3& color)
        : m_Color(color), m_Cube()
    {
        m_Cube.SetPosition(position);
        m_Cube.SetScale(glm::vec3(0.2f)); // Scale down for a smaller light representation
        m_Cube.SetColor(color);
        m_SpecularIntensity = 0.5f;

        std::string projectRoot = GetProjectRoot();
        auto light_cube_vs = projectRoot.append("\\src\\Resources\\Shaders\\light_cube.vs");
        projectRoot = GetProjectRoot();
        auto light_cube_fs = projectRoot.append("\\src\\Resources\\Shaders\\light_cube.fs");

        m_Shader =  new Shader(light_cube_vs.c_str(), light_cube_fs.c_str());
    }
        
    Light::~Light()
    {
    
    }


    void Light::Update()
    {
        // Update light logic (e.g., animations) if necessary
    }

    void Light::Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection)
    {
        m_Shader->Bind();
        m_Shader->setMat4("model", m_Cube.GetModelMatrix());
        m_Shader->setMat4("view", view);
        m_Shader->setMat4("projection", projection);
        m_Shader->setVec3( "lightColor", m_Color);

        shader.Bind();
        shader.setVec3("lightColor", m_Color);
        shader.setVec3("lightPos", m_Cube.GetPosition());
        shader.setFloat("specularIntensity", m_SpecularIntensity);

        renderer.Render(m_Cube.GetCubeVA(), m_Cube.GetVertexCount(), *m_Shader);
    }
}
