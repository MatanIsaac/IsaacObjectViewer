#include "Cylinder.h"
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace isaacGraphicsEngine
{
    Cylinder::Cylinder(const glm::vec3& position) 
        : m_Position(position), m_Color(DEFAULT_COLOR), m_Scale(glm::vec3(1.0f))
    {
        // Generate cylinder data
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        m_VertexCount = GenerateCylinder(0.5f, 0.5f, 1.0f, 36, vertices, indices);
        m_IndexCount = static_cast<unsigned int>(indices.size());

        // Create the VertexArray
        m_VertexArray = std::make_unique<VertexArray>();

        // Create the VertexBuffer and store it as a member
        m_VertexBuffer = std::make_unique<VertexBuffer>(vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof(float)));

        // Create the IndexBuffer
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices.data(), m_IndexCount);

        // Define the vertex layout: 3 floats for position and 3 floats for normal
        VertexBufferLayout layout;
        layout.Push<float>(3); // position
        layout.Push<float>(3); // normal

        m_VertexArray->AddBuffer(*m_VertexBuffer, layout);
        m_VertexArray->Unbind();

        m_Color = DEFAULT_COLOR;
    }

    Cylinder::~Cylinder()
    {
        // Unique pointers automatically clean up resources.
    }

    void Cylinder::Update()
    {
        // Add update logic (animations, etc.) if necessary.
    }

    void Cylinder::Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection)
    {        
        // Build the model matrix: translate and scale as needed.
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_Position);
        model = glm::scale(model, m_Scale);

        // Set uniform values
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("objectColor", m_Color);
        
        // Render using indexed drawing.
        renderer.Render(*m_VertexArray, *m_IndexBuffer, shader);
    }
  
    int Cylinder::GenerateCylinder(float baseRadius, float topRadius, float height, int sectorCount,
                                     std::vector<float>& vertices, std::vector<unsigned int>& indices)
    {
        vertices.clear();
        indices.clear();

        float halfHeight = height * 0.5f;
        float sectorStep = 2.0f * glm::pi<float>() / sectorCount;

        // Generate side vertices
        for (int i = 0; i <= sectorCount; ++i)
        {
            float sectorAngle = i * sectorStep;
            float x = cos(sectorAngle);
            float y = sin(sectorAngle);

            // Bottom vertex (position and normal)
            vertices.push_back(baseRadius * x); // x
            vertices.push_back(baseRadius * y); // y
            vertices.push_back(-halfHeight);      // z
            vertices.push_back(x);                // normal x
            vertices.push_back(y);                // normal y
            vertices.push_back(0.0f);             // normal z

            // Top vertex (position and normal)
            vertices.push_back(topRadius * x);    // x
            vertices.push_back(topRadius * y);    // y
            vertices.push_back(halfHeight);       // z
            vertices.push_back(x);                // normal x
            vertices.push_back(y);                // normal y
            vertices.push_back(0.0f);             // normal z
        }

        // Generate side indices
        int k1, k2;
        for (int i = 0; i < sectorCount; ++i)
        {
            k1 = i * 2;       // current bottom vertex index
            k2 = k1 + 2;      // next bottom vertex index

            // Two triangles per sector
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }

        // Generate bottom cap
        int baseCenterIndex = static_cast<int>(vertices.size() / 6);
        // Center vertex for the bottom cap
        vertices.push_back(0.0f); // x
        vertices.push_back(0.0f); // y
        vertices.push_back(-halfHeight); // z
        vertices.push_back(0.0f); // normal x
        vertices.push_back(0.0f); // normal y
        vertices.push_back(-1.0f); // normal z

        for (int i = 0; i < sectorCount; ++i)
        {
            indices.push_back(baseCenterIndex);
            indices.push_back(i * 2);
            indices.push_back(((i + 1) % sectorCount) * 2);
        }

        // Generate top cap
        int topCenterIndex = static_cast<int>(vertices.size() / 6);
        // Center vertex for the top cap
        vertices.push_back(0.0f); // x
        vertices.push_back(0.0f); // y
        vertices.push_back(halfHeight); // z
        vertices.push_back(0.0f); // normal x
        vertices.push_back(0.0f); // normal y
        vertices.push_back(1.0f); // normal z

        for (int i = 0; i < sectorCount; ++i)
        {
            indices.push_back(topCenterIndex);
            indices.push_back(i * 2 + 1);
            indices.push_back(((i + 1) % sectorCount) * 2 + 1);
        }

        return static_cast<int>(vertices.size() / 6); // Return the vertex count.
    }
}
