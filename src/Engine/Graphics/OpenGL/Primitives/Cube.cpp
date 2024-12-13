#include "Cube.h"
// #include "Utility/GLErrorManager.h"

#include <glad/glad.h>

#include <vector>

#include "Utility/config.h"

namespace isaacGraphicsEngine
{
    Cube::Cube()
    {
        size_t numFloats = sizeof(m_CubeVertices) / sizeof(m_CubeVertices[0]);
        m_VertexCount = numFloats / 6;

        GLuint cube_indices[] = {
            0, 1, 2, 2, 3, 0,       // front face
            4, 5, 6, 6, 7, 4,       // back face
            8, 9, 10, 10, 11, 8,    // left face
            12, 13, 14, 14, 15, 12, // right face
            16, 17, 18, 18, 19, 16, // top face
            20, 21, 22, 22, 23, 20  // bottom face
        };
        m_IndicesCount = sizeof(cube_indices) / sizeof(cube_indices[0]);

        // Create and bind buffers using smart pointers
        m_VertexArray = std::make_unique<VertexArray>();
        m_VertexBuffer = std::make_unique<VertexBuffer>(m_CubeVertices, sizeof(m_CubeVertices));
        m_IndexBuffer = std::make_unique<IndexBuffer>(cube_indices, m_IndicesCount);

        VertexBufferLayout vb_layout;
        vb_layout.Push<float>(3); // Positions
        vb_layout.Push<float>(3); // Normals
        m_VertexArray->AddBuffer(*m_VertexBuffer, vb_layout);

        m_Position = DEFAULT_POSITION;
        m_Color = DEFAULT_COLOR;
        m_Scale = glm::vec3(1.0f);
        m_Rotation = glm::vec3(0.0f);
    }

    Cube::~Cube()
    {
        // Smart pointers handle cleanup automatically
    }

    void Cube::Update()
    {
        // Update logic, if any (e.g., animations)
    }

    void Cube::Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection)
    {
        // Calculate the model matrix
        auto model = GetModelMatrix();

        // Bind the shader and set uniforms
        //shader.Bind(); - binded before this function call.
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("objectColor", m_Color);

        // Bind the vertex array and draw the cube
        m_VertexArray->Bind();
        renderer.Render(*m_VertexArray, m_VertexCount, shader);
    }

    void Cube::SetScale(const glm::vec3& scale)
    {
        m_Scale = scale;
    }

    void Cube::SetRotation(const glm::vec3& rotation)
    {
        m_Rotation = rotation;
    }

    glm::mat4 Cube::GetModelMatrix() const
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
