#include "Sphere.h"
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{
    Sphere::Sphere(const glm::vec3& position) 
        : m_ID(GenerateUniqueID())
        , m_Name("Sphere_" + std::to_string(m_ID))
        , m_Position(position)
        , m_Scale(glm::vec3(1.0f))
        , m_Color(DEFAULT_COLOR)
        
    {
        // Adjust these parameters as needed for detail
        float radius = 0.5f;
        int sectorCount = 36; // longitude divisions
        int stackCount  = 18; // latitude divisions

        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        m_VertexCount = GenerateSphere(radius, sectorCount, stackCount, vertices, indices);
        m_IndexCount = static_cast<unsigned int>(indices.size());

        // Create the VertexArray
        m_VertexArray = std::make_unique<VertexArray>();

        // Create the VertexBuffer from the generated vertex data
        m_VertexBuffer = std::make_unique<VertexBuffer>(vertices.data(), 
                                    static_cast<unsigned int>(vertices.size() * sizeof(float)));

        // Create the IndexBuffer from the generated indices
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices.data(), m_IndexCount);

        // Define the vertex layout: 3 floats for position and 3 floats for normal
        VertexBufferLayout layout;
        layout.Push<float>(3); // position
        layout.Push<float>(3); // normal

        m_VertexArray->AddBuffer(*m_VertexBuffer, layout);
        m_VertexArray->Unbind();
    }

    Sphere::~Sphere()
    {
        // Unique pointers will auto-cleanup.
    }

    void Sphere::Update()
    {
        // Update logic (animations, physics, etc.) goes here.
    }

    void Sphere::Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader)
    {        
        if(shader == nullptr)
        {
            LOG_INFO("Can't Render shader parameters is null!");
            return;
        }
        
        // Build the model matrix using the current position and scale.
        glm::mat4 model = GetModelMatrix();
        
        // Set uniform values for the shader.
        shader->Bind();
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setVec3("objectColor", m_Color);
        
        // Render using indexed drawing.
        renderer.Render(*m_VertexArray, *m_IndexBuffer, *shader);
    }
  
    int Sphere::GenerateSphere(float radius, int sectorCount, int stackCount,
                                 std::vector<float>& vertices, std::vector<unsigned int>& indices)
    {
        vertices.clear();
        indices.clear();

        float x, y, z;            // vertex position
        float nx, ny, nz, lengthInv = 1.0f / radius; // normal
        float sectorStep = 2 * glm::pi<float>() / sectorCount;
        float stackStep = glm::pi<float>() / stackCount;
        float sectorAngle, stackAngle;

        // Generate vertices for each stack and sector.
        for (int i = 0; i <= stackCount; ++i)
        {
            // stackAngle: 0 -> pi
            stackAngle = i * stackStep;
            float sinStack = sin(stackAngle);
            float cosStack = cos(stackAngle);

            // Loop through each sector.
            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;

                // Vertex position (x, y, z)
                x = radius * sinStack * cos(sectorAngle);
                y = radius * cosStack;
                z = radius * sinStack * sin(sectorAngle);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // Normalized vertex normal (since the sphere is centered at the origin)
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);
            }
        }

        // Generate indices for the sphere mesh (two triangles per quad).
        int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;        // beginning of next stack

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // First triangle (except for the top stack)
                if (i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                // Second triangle (except for the bottom stack)
                if (i != (stackCount - 1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        // Return the number of vertices (each vertex consists of 6 floats: 3 for position, 3 for normal)
        return static_cast<int>(vertices.size() / 6);
    }

    glm::mat4 Sphere::GetModelMatrix() const
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
