#include "Plane.h"


namespace isaacObjectLoader
{
    Plane::Plane(const glm::vec3& position) 
        : m_Position(position)
    {
        // Initialize vertex count for indexed drawing
        m_VertexCount = m_IndicesCount;

        // Create the VertexArray
        m_VertexArray = std::make_unique<VertexArray>();

        // Create the VertexBuffer with the vertex data
        m_VertexBuffer = std::make_unique<VertexBuffer>(m_PlaneVertices, sizeof(m_PlaneVertices));

        // Set up the VertexBufferLayout
        VertexBufferLayout vb_layout;
        vb_layout.Push<float>(3); // Positions
        vb_layout.Push<float>(3); // Normals
        m_VertexArray->AddBuffer(*m_VertexBuffer, vb_layout);

        // Create the IndexBuffer with the index data
        m_IndexBuffer = std::make_unique<IndexBuffer>(m_PlaneIndices, m_IndicesCount);

        m_Color = DEFAULT_COLOR;
        m_Scale = glm::vec3(1.0f);
        m_Rotation = glm::vec3(0.0f);
    }

    Plane::~Plane()
    {
        
    }

    void Plane::Update()
    {
        
    }

    void Plane::Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection)
    {
        // Calculate the model matrix
        auto model = GetModelMatrix();

        // Bind the shader and set uniforms
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("objectColor", m_Color);

        // indexed drawing
        renderer.Render(*m_VertexArray, *m_IndexBuffer, shader); 
        /*renderer.Render(*m_VertexArray, m_VertexCount, shader); */
    }

    void Plane::SetScale(const glm::vec3& scale)
    {
        m_Scale = scale;
    }

    void Plane::SetRotation(const glm::vec3& rotation)
    {
        m_Rotation = rotation;
    }

    glm::mat4 Plane::GetModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_Position);
        model = glm::rotate(model, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, m_Scale);
        return model;
    }
}
