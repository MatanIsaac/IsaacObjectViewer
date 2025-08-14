#include "Plane.h"
#include "Utility/Log.hpp"
#include "TextureManager.h"
#include "Core/Engine.h"

namespace isaacObjectViewer
{
    Plane::Plane(const glm::vec3& position) 
        : m_ID(GenerateUniqueID())
        , m_Name("Plane_" + std::to_string(m_ID)) 
        , m_Position(position)
        , m_Rotation(0.0f)
        , m_Orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
        , m_Scale(1.0f)
        , m_Color(DEFAULT_COLOR)
        , m_UseMaterial(false)
        ,m_Material(GetDefaultMaterial())
    {
        // Initialize vertex count for indexed drawing
        m_VertexCount = 4;

        // Create the VertexArray
        m_VertexArray = std::make_unique<VertexArray>();

        // Create the VertexBuffer with the vertex data
        m_VertexBuffer = std::make_unique<VertexBuffer>(m_PlaneVertices, sizeof(m_PlaneVertices));

        // Set up the VertexBufferLayout
        VertexBufferLayout vb_layout;
        vb_layout.Push<float>(3); // Positions
        vb_layout.Push<float>(3); // Normals
        vb_layout.Push<float>(2); // TexCoords
        m_VertexArray->AddBuffer(*m_VertexBuffer, vb_layout);

        // Create the IndexBuffer with the index data
        m_IndexBuffer = std::make_unique<IndexBuffer>(m_PlaneIndices, m_IndicesCount);
    }

    Plane::~Plane()
    {}

    void Plane::Update()
    {}

    void Plane::Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader)
    {
        if(shader == nullptr)
        {
            LOG_INFO("Shader parameter is null!");
            return;
        }
        
        // Calculate the model matrix
        auto model = GetModelMatrix();

        // Bind the shader and set uniforms
        shader->Bind();
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setVec3("objectColor", m_Color);
        
        bool hasDiffuse  = (m_Material.Diffuse  != nullptr);
        bool hasSpecular = (m_Material.Specular != nullptr);

        const bool useMaterial = m_UseMaterial && (hasDiffuse || hasSpecular);

        shader->setBool("useMaterial",   useMaterial);
        shader->setBool("hasDiffuseMap",  hasDiffuse);
        shader->setBool("hasSpecularMap", hasSpecular);
        shader->setFloat("material.shininess",m_Material.Shininess);
        
        if (useMaterial) 
        {
            glActiveTexture(GL_TEXTURE0);
            if (hasDiffuse) 
            {
                m_Material.Diffuse->Bind(); 
            }    
            else
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }   
            shader->setInt("material.diffuse", 0);

            glActiveTexture(GL_TEXTURE1);
            if (hasSpecular) 
            {
                m_Material.Specular->Bind(); 
            } 
            else 
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            shader->setInt("material.specular", 1);
        } 
        else 
        {
            shader->setVec3("objectColor", m_Color);
        }

        renderer.Render(*m_VertexArray, *m_IndexBuffer, *shader);
        glActiveTexture(GL_TEXTURE0);
    }

}
