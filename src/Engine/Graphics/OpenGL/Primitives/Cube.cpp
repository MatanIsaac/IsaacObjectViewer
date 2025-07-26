#include "Cube.h"
#include "Utility/Log.hpp"
#include "TextureManager.h"

namespace isaacObjectViewer
{
    Cube::Cube(const glm::vec3& position)
        : m_ID(GenerateUniqueID())
        , m_Name("Cube_" + std::to_string(m_ID))
        , m_Position(position)
    {
        // Set index count for our cube (36 indices)
        m_IndicesCount = 36;

        // Create the VertexArray
        m_VertexArray = std::make_unique<VertexArray>();

        // Create the VertexBuffer with our vertex data
        m_VertexBuffer = std::make_unique<VertexBuffer>(m_CubeVertices, sizeof(m_CubeVertices));

        // Define the vertex layout: 3 floats for position, 3 for normal
        VertexBufferLayout vb_layout;
        vb_layout.Push<float>(3); // Positions
        vb_layout.Push<float>(3); // Normals
        vb_layout.Push<float>(2); // TexCoords
        m_VertexArray->AddBuffer(*m_VertexBuffer, vb_layout);

        // Create the IndexBuffer with our index data
        m_IndexBuffer = std::make_unique<IndexBuffer>(m_CubeIndices, m_IndicesCount);

        m_Color = DEFAULT_COLOR;
        m_Scale = glm::vec3(1.0f);
        m_Rotation = glm::vec3(0.0f);
        m_Orientation = glm::quat(glm::radians(m_Rotation));

        
        std::string path = "/src/Resources/Textures/wall.jpg";
        ConvertSeparators(path);
        
        m_Texture = TextureManager::LoadTexture(path,false);
    }

    Cube::~Cube()
    {
        // Resources are managed by unique_ptr.
    }

    void Cube::Update()
    {
        // (Update logic if necessary)
    }

    void Cube::Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader)
    {
        if(shader == nullptr)
        {
            LOG_INFO("Can't Render shader parameters is null!");
            return;
        }
        
        // Compute model matrix
        auto model = GetModelMatrix();

        // Bind shader and set uniforms
        shader->Bind();
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setVec3("objectColor", m_Color);
        shader->setInt("diffuseTexture", 0); // set sampler to use texture unit 0
        if(m_Texture)
        {
            m_Texture->Bind();
            shader->setBool("useTexture", true);
        }
        else
        {
            shader->setBool("useTexture", false);
        }

        // Render using indexed drawing:
        renderer.Render(*m_VertexArray, *m_IndexBuffer, *shader);
    }
}
