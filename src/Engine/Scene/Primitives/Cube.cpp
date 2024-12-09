#include "Cube.h"
// #include "Utility/GLErrorManager.h"

#include <glad/glad.h>

#include <vector>

#include "Utility/config.h"

namespace isaacObjectLoader
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
        vb_layout.Push<float>(3);
        vb_layout.Push<float>(3);
        m_VertexArray->AddBuffer(*m_VertexBuffer, vb_layout);
    }

    Cube::~Cube()
    {
        // Smart pointers handle cleanup
    }

    void Cube::Update() {}

} // namespace isaacObjectLoader