#include "Mesh.h"
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{
    Mesh::Mesh(const std::vector<Vertex>& vertices, 
               const std::vector<unsigned int>& indices, 
               const std::vector<std::shared_ptr<Texture>>& textures, 
               Material material)
        : vertices(vertices)
        , indices(indices)
        , textures(textures)
        , m_Material(material)
        , m_BBoxMin(glm::vec3( std::numeric_limits<float>::max())) 
        , m_BBoxMax(-glm::vec3( std::numeric_limits<float>::max())) 
    {
        SetupMesh();
    }

    void Mesh::Draw(Shader& shader)
    {
        // prefer explicit Material, else fall back to imported textures
        std::shared_ptr<Texture> diff = m_Material.Diffuse;
        std::shared_ptr<Texture> spec = m_Material.Specular;

        if (!diff) 
        {
            for (auto& t : textures) if (t->GetType() == TextureType::DIFFUSE)  { diff = t; break; }
        }
        if (!spec) 
        {
            for (auto& t : textures) if (t->GetType() == TextureType::SPECULAR) { spec = t; break; }
        }

        const bool useMat = (bool)diff;

        shader.setBool("useMaterial", useMat);
        shader.setFloat("material.shininess", m_Material.Shininess);

        if (useMat) {
            // diffuse at unit 0
            glActiveTexture(GL_TEXTURE0);
            diff->Bind();
            shader.setInt("material.diffuse", 0);

            // specular at unit 1 (bind a white texture or unbind if you don’t have one)
            glActiveTexture(GL_TEXTURE1);
            if (spec) spec->Bind(); else glBindTexture(GL_TEXTURE_2D, 0);
            shader.setInt("material.specular", 1);
        } else {
            // No textures: fall back to objectColor path in the shader
            shader.setVec3("objectColor", glm::vec3(1.0f));
        }

        // 3) Draw
        m_VertexArray->Bind();
        m_IndexBuffer->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }


    void Mesh::SetupMesh()
    {
        m_VertexArray = std::make_unique<VertexArray>();
        m_VertexBuffer = std::make_unique<VertexBuffer>(vertices.data(),vertices.size() * sizeof(Vertex));
        m_IndexBuffer = std::make_unique<IndexBuffer>(&indices[0], indices.size());

        m_VertexArray->Bind();
        m_VertexBuffer->Bind();
        m_IndexBuffer->Bind();

        VertexBufferLayout vb_layout;
        vb_layout.Push<float>(3); // Position
        vb_layout.Push<float>(3); // Normal
        vb_layout.Push<float>(2); // TexCoords
        vb_layout.Push<float>(3); // Tangent
        vb_layout.Push<float>(3); // Bitangent
        vb_layout.Push<unsigned int>(4);   // Bone IDs
        vb_layout.Push<float>(4); // Weights
        m_VertexArray->AddBuffer(*m_VertexBuffer, vb_layout);

        glBindVertexArray(0);

        // Compute AABB from positions in 'vertices'
        for (const auto& v : vertices)
        {
            m_BBoxMin = glm::min(m_BBoxMin, v.Position);
            m_BBoxMax = glm::max(m_BBoxMax, v.Position);
        }
    }
}