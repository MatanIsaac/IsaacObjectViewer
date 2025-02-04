#include "Cylinder.h"

namespace isaacGraphicsEngine
{
    Cylinder::Cylinder(const glm::vec3& position) 
        : m_Position(position), m_Color(DEFAULT_COLOR)
    {
        // Generate cylinder data
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        m_VertexCount = GenerateCylinder(0.5f, 0.5f, 1.0f, 36, vertices, indices);
        
        m_VertexArray = std::make_unique<VertexArray>();
        VertexBuffer vb(vertices.data(), (unsigned int) (vertices.size() * sizeof(float)));
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices.data(), indices.size());
        VertexBufferLayout layout;
        layout.Push<float>(3); // position
        layout.Push<float>(3); // normal
        m_VertexArray->AddBuffer(vb, layout);
        m_VertexArray->Unbind();

        m_Color = DEFAULT_COLOR;
        m_Scale = glm::vec3(1.0f);
    }

    Cylinder::~Cylinder()
    {
        // Smart pointers handle cleanup automatically
    }

    void Cylinder::Update()
    {
        // Update logic, if any (e.g., animations)
    }

    void Cylinder::Render(const Renderer& renderer, Shader& shader, const glm::mat4& view, const glm::mat4& projection)
    {        
        // Model matrix (identity)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_Position);
        model = glm::scale(model, m_Scale); // Adjust scale if necessary.

        // Set uniform values
        shader.setMat4("model",model );
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("objectColor", m_Color);
        
        renderer.Render(*m_VertexArray, *m_IndexBuffer, shader);
    }
  
    int Cylinder::GenerateCylinder(float baseRadius, float topRadius, float height, int sectorCount,
                                std::vector<float>& vertices, std::vector<unsigned int>& indices)
    {
        vertices.clear();
        indices.clear();

        float halfHeight = height * 0.5f;
        float sectorStep = 2.0f * glm::pi<float>() / sectorCount;
        float lengthInv = 1.0f / height; // Used for normal calculation.

        // Generate side vertices
        for (int i = 0; i <= sectorCount; ++i)
        {
            float sectorAngle = i * sectorStep;
            float x = cos(sectorAngle);
            float y = sin(sectorAngle);

            // Bottom vertex
            vertices.push_back(baseRadius * x); // Position
            vertices.push_back(baseRadius * y);
            vertices.push_back(-halfHeight);
            vertices.push_back(x); // Normal
            vertices.push_back(y);
            vertices.push_back(0.0f);

            // Top vertex
            vertices.push_back(topRadius * x); // Position
            vertices.push_back(topRadius * y);
            vertices.push_back(halfHeight);
            vertices.push_back(x); // Normal
            vertices.push_back(y);
            vertices.push_back(0.0f);
        }

        // Generate side indices
        int k1, k2;
        for (int i = 0; i < sectorCount; ++i)
        {
            k1 = i * 2;
            k2 = k1 + 2;

            // Two triangles per sector
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }

        // Generate bottom cap
        int baseCenterIndex = vertices.size() / 6; // Each vertex has 6 floats.
        vertices.push_back(0.0f); // Position
        vertices.push_back(0.0f);
        vertices.push_back(-halfHeight);
        vertices.push_back(0.0f); // Normal
        vertices.push_back(0.0f);
        vertices.push_back(-1.0f);

        for (int i = 0; i < sectorCount; ++i)
        {
            indices.push_back(baseCenterIndex);
            indices.push_back(i * 2);
            indices.push_back(((i + 1) % sectorCount) * 2);
        }

        // Generate top cap
        int topCenterIndex = vertices.size() / 6; // Each vertex has 6 floats.
        vertices.push_back(0.0f); // Position
        vertices.push_back(0.0f);
        vertices.push_back(halfHeight);
        vertices.push_back(0.0f); // Normal
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);

        for (int i = 0; i < sectorCount; ++i)
        {
            indices.push_back(topCenterIndex);
            indices.push_back(i * 2 + 1);
            indices.push_back(((i + 1) % sectorCount) * 2 + 1);
        }

        return static_cast<int>(vertices.size() / 6); // Return the vertex count.
    }



}
