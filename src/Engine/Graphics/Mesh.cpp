#include "Mesh.h"
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{
    Mesh::Mesh(const std::vector<Vertex>& vertices,
             const std::vector<unsigned int>& indices,
             const std::vector<std::shared_ptr<Texture>>& textures, 
             Material material, const std::string& name)
            : m_Vertices(vertices)
            , m_Indices(indices)
            , m_Textures(textures)
            , m_Material(material)
            , m_Name(name)
            , m_Position(DEFAULT_POSITION)
            , m_Rotation(DEFAULT_ROTATION)
            , m_Orientation(glm::quat(glm::radians(m_Rotation)))
            , m_Scale(DEFAULT_SCALE)
            , m_BBoxMin(std::numeric_limits<float>::max())      
            , m_BBoxMax(std::numeric_limits<float>::lowest()) 
    {
        SetupMesh();
    }


    Mesh::Mesh(const Mesh& other)
        : m_Vertices(other.m_Vertices)
        , m_Indices(other.m_Indices)
        , m_Textures(other.m_Textures)
        , m_Material(other.m_Material)
        , m_Name(other.m_Name)                 // NEW
        , m_Position(other.m_Position)         // NEW
        , m_Rotation(other.m_Rotation)         // NEW
        , m_Orientation(other.m_Orientation)   // NEW
        , m_Scale(other.m_Scale)               // NEW
        , m_BBoxMin(other.m_BBoxMin)
        , m_BBoxMax(other.m_BBoxMax)
    {
        // Rebuild GL objects from CPU data
        SetupMesh();
    }


    Mesh& Mesh::operator=(const Mesh& other)
    {
        if (this != &other)
        {
            m_Vertices   = other.m_Vertices;
            m_Indices    = other.m_Indices;
            m_Textures   = other.m_Textures;
            m_Material   = other.m_Material;
            m_Name       = other.m_Name;               
            m_Position   = other.m_Position;           
            m_Rotation   = other.m_Rotation;           
            m_Orientation= other.m_Orientation;        
            m_Scale      = other.m_Scale;                  
            // Rebuild GL objects
            m_IndexBuffer.reset();
            m_VertexBuffer.reset();
            m_VertexArray.reset();
            m_BBoxMin    = other.m_BBoxMin;
            m_BBoxMax    = other.m_BBoxMax;
        
            SetupMesh();
        }
        return *this;
    }

    Mesh::Mesh(Mesh&& other) noexcept
        : m_Vertices(std::move(other.m_Vertices))
        , m_Indices(std::move(other.m_Indices))
        , m_Textures(std::move(other.m_Textures))
        , m_Material(std::move(other.m_Material))
        , m_Name(std::move(other.m_Name))               
        , m_Position(other.m_Position)                  
        , m_Rotation(other.m_Rotation)                  
        , m_Orientation(other.m_Orientation)            
        , m_Scale(other.m_Scale)                        
        , m_IndexBuffer(std::move(other.m_IndexBuffer))
        , m_VertexBuffer(std::move(other.m_VertexBuffer))
        , m_VertexArray(std::move(other.m_VertexArray))
        , m_BBoxMin(other.m_BBoxMin)
        , m_BBoxMax(other.m_BBoxMax)
    {}
    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        if (this != &other)
        {
            m_Vertices      = std::move(other.m_Vertices);
            m_Indices       = std::move(other.m_Indices);
            m_Textures      = std::move(other.m_Textures);
            m_Material    = std::move(other.m_Material);
            m_IndexBuffer = std::move(other.m_IndexBuffer);
            m_VertexBuffer= std::move(other.m_VertexBuffer);
            m_VertexArray = std::move(other.m_VertexArray);
            m_BBoxMin     = other.m_BBoxMin;
            m_BBoxMax     = other.m_BBoxMax;
        }
        return *this;
    }

    void Mesh::Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader)
    {
        if (!m_VertexArray || !m_VertexBuffer || (!m_IndexBuffer && GetIndexCount() > 0))
        {
            LOG_ERROR("Can't Render Mesh: invalid VAO/VBO/IBO");
            return;
        }

        shader->Bind();
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setMat4("model", GetModelMatrix());

        // Select material textures (prefer explicit Material, fallback to mesh textures)
        std::shared_ptr<Texture> diffuse = m_Material.Diffuse;
        std::shared_ptr<Texture> specular = m_Material.Specular;

        if (!diffuse) 
        {
            for (const auto& t : m_Textures)
                if (t && t->GetType() == TextureType::DIFFUSE) 
                { 
                    diffuse = t; 
                    break;
                }
        }
        if (!specular) 
        {
            for (const auto& t : m_Textures)
                if (t && t->GetType() == TextureType::SPECULAR) 
                { 
                    specular = t; 
                    break; 
                }
        }

        const bool useMaterial = (bool)diffuse;
        shader->setBool("useMaterial", useMaterial);
        shader->setFloat("material.shininess", m_Material.Shininess);

        if (useMaterial) 
        {
            // Bind diffuse texture to unit 0
            glActiveTexture(GL_TEXTURE0);
            diffuse->Bind();
            shader->setInt("material.diffuse", 0);

            // Bind specular texture to unit 1 (or unbind if missing)
            glActiveTexture(GL_TEXTURE1);
            if (specular) specular->Bind(); else glBindTexture(GL_TEXTURE_2D, 0);
            shader->setInt("material.specular", 1);
        } else {
            shader->setVec3("objectColor", glm::vec3(1.0f));
        }

        // Use renderer to draw
        renderer.Render(*m_VertexArray, *m_IndexBuffer, *shader);

        // Reset texture unit
        glActiveTexture(GL_TEXTURE0);
    }
    void Mesh::RenderWithParent(const Renderer& renderer,
                            const glm::mat4& parentModel,
                            const glm::mat4& view,
                            const glm::mat4& projection,
                            Shader* shader)
    {
        if (!shader || !m_VertexArray || !m_IndexBuffer)
        {
            LOG_ERROR("Can't Render Mesh: invalid shader or buffers");
            return;
        }

        shader->Bind();
        shader->setMat4("view",       view);
        shader->setMat4("projection", projection);

        // key line: parent * local
        const glm::mat4 model = parentModel * GetModelMatrix();
        shader->setMat4("model", model);

        // --- same material binding logic as in Mesh::Render() ---
        std::shared_ptr<Texture> diffuse  = m_Material.Diffuse;
        std::shared_ptr<Texture> specular = m_Material.Specular;

        if (!diffuse)
            for (const auto& t : m_Textures) if (t && t->GetType()==TextureType::DIFFUSE)  { diffuse=t;  break; }
        if (!specular)
            for (const auto& t : m_Textures) if (t && t->GetType()==TextureType::SPECULAR) { specular=t; break; }

        const bool useMaterial = (bool)diffuse;
        shader->setBool ("useMaterial", useMaterial);
        shader->setFloat("material.shininess", m_Material.Shininess);

        if (useMaterial)
        {
            glActiveTexture(GL_TEXTURE0); diffuse->Bind();
            shader->setInt("material.diffuse", 0);

            glActiveTexture(GL_TEXTURE1);
            if (specular) specular->Bind(); else glBindTexture(GL_TEXTURE_2D, 0);
            shader->setInt("material.specular", 1);
        }
        else
        {
            shader->setVec3("objectColor", glm::vec3(1.0f));
        }

        renderer.Render(*m_VertexArray, *m_IndexBuffer, *shader);
        glActiveTexture(GL_TEXTURE0);
    }


    void Mesh::SetupMesh()
    {
        // Guard: no vertices => nothing to build
        if (m_Vertices.empty())
        {
            m_VertexArray.reset();
            m_VertexBuffer.reset();
            m_IndexBuffer.reset();
            m_BBoxMin = glm::vec3(0.0f);
            m_BBoxMax = glm::vec3(0.0f);
            return;
        }

        m_VertexArray  = std::make_unique<VertexArray>();
        m_VertexBuffer = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

        // Guard indices
        if (!m_Indices.empty())
            m_IndexBuffer = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());
        else
            m_IndexBuffer.reset(); // renderer should handle draw-arrays or skip

        m_VertexArray->Bind();
        m_VertexBuffer->Bind();
        if (m_IndexBuffer) m_IndexBuffer->Bind();

        VertexBufferLayout vb_layout;
        vb_layout.Push<float>(3);         // Position
        vb_layout.Push<float>(3);         // Normal
        vb_layout.Push<float>(2);         // TexCoords
        vb_layout.Push<float>(3);         // Tangent
        vb_layout.Push<float>(3);         // Bitangent
        vb_layout.Push<unsigned int>(4);  // Bone IDs (make sure this ends up as integer attrib)
        vb_layout.Push<float>(4);         // Weights
        m_VertexArray->AddBuffer(*m_VertexBuffer, vb_layout);

        glBindVertexArray(0);

        // Tight AABB
        m_BBoxMin = m_BBoxMax = m_Vertices[0].Position;
        for (size_t i = 1; i < m_Vertices.size(); ++i)
        {
            m_BBoxMin = glm::min(m_BBoxMin, m_Vertices[i].Position);
            m_BBoxMax = glm::max(m_BBoxMax, m_Vertices[i].Position);
        }
    }

}