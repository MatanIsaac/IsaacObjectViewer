#include "Cylinder.h"
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utility/Log.hpp"

namespace isaacObjectViewer
{
    Cylinder::Cylinder(const glm::vec3& position) 
        : m_ID(GenerateUniqueID())
        , m_Name("Cylinder_" + std::to_string(m_ID))
        , m_Position(position)
        , m_Rotation(DEFAULT_ROTATION)
        , m_Scale(glm::vec3(1.0f))
        , m_Color(DEFAULT_COLOR) 
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
        m_Orientation = glm::quat(glm::radians(m_Rotation));
    }

    Cylinder::~Cylinder()
    {
        // Unique pointers automatically clean up resources.
    }

    void Cylinder::Update()
    {
        // Add update logic (animations, etc.) if necessary.
    }

    void Cylinder::Render(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, Shader* shader)
    {        
        if(shader == nullptr)
        {
            LOG_INFO("Shader parameter is null!");
            return;
        }

        // Build the model matrix: translate and scale as needed.
        glm::mat4 model = GetModelMatrix();

        // Set uniform values
        shader->Bind();
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setVec3("objectColor", m_Color);
        shader->setBool("useMaterial", false);
        //shader->setFloat("material.shininess",m_Material.Shininess);


        // Render using indexed drawing.
        renderer.Render(*m_VertexArray, *m_IndexBuffer, *shader);
    }
  
    int Cylinder::GenerateCylinder(float baseRadius, float topRadius, float height, int sectorCount,
        std::vector<float>& vertices, std::vector<unsigned int>& indices)
    {
        // Clear any previous data.
        vertices.clear();
        indices.clear();

        // Precompute common values.
        float halfHeight = height * 0.5f;
        float sectorStep = 2.0f * glm::pi<float>() / sectorCount;

        // Compute the slope for the side (used for normals)
        // For a conical frustum, dr/dz = (baseRadius - topRadius) / height.
        float dr = (baseRadius - topRadius) / height;
        // The length of the vector (1, 0, dr) is sqrt(1 + dr^2). This factor will normalize the side normal.
        float sideNormalFactor = std::sqrt(1.0f + dr * dr);

        // =========================
        // 1. Generate Side Vertices
        // =========================
        // We generate (sectorCount+1) pairs of vertices (bottom and top) so that the first and last are identical.
        // Each vertex has 6 floats: (x, y, z, nx, ny, nz).
        int sideVertexStart = static_cast<int>(vertices.size() / 6);
        for (int i = 0; i <= sectorCount; ++i)
        {
            float theta = i * sectorStep;
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            // --- Compute the side normal.
            // For a conical frustum the unnormalized normal would be (cosTheta, sinTheta, -dr).
            // Normalize it:
            float nx = cosTheta / sideNormalFactor;
            float ny = sinTheta / sideNormalFactor;
            float nz = -dr / sideNormalFactor;

            // Bottom vertex (at z = -halfHeight) using baseRadius.
            float bx = baseRadius * cosTheta;
            float by = baseRadius * sinTheta;
            float bz = -halfHeight;
            vertices.push_back(bx);  // position x
            vertices.push_back(by);  // position y
            vertices.push_back(bz);  // position z
            vertices.push_back(nx);  // normal x
            vertices.push_back(ny);  // normal y
            vertices.push_back(nz);  // normal z

            // Top vertex (at z = halfHeight) using topRadius.
            float tx = topRadius * cosTheta;
            float ty = topRadius * sinTheta;
            float tz = halfHeight;
            vertices.push_back(tx);  // position x
            vertices.push_back(ty);  // position y
            vertices.push_back(tz);  // position z
            vertices.push_back(nx);  // normal x (same as bottom)
            vertices.push_back(ny);  // normal y
            vertices.push_back(nz);  // normal z
        }

        // -------------------------
        // Generate Side Indices
        // -------------------------
        // Each sector (quad) is made of two triangles.
        for (int i = 0; i < sectorCount; ++i)
        {
            int k1 = sideVertexStart + i * 2;     // current bottom vertex index
            int k2 = k1 + 2;                      // next bottom vertex index

            // First triangle of the quad.
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            // Second triangle of the quad.
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }

        // ========================
        // 2. Generate Bottom Cap
        // ========================
        // For a sharp edge between side and cap, we generate separate vertices for the cap rim.
        int bottomCapVertexStart = static_cast<int>(vertices.size() / 6);
        for (int i = 0; i <= sectorCount; ++i)
        {
            float theta = i * sectorStep;
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            // Position on the bottom rim.
            float bx = baseRadius * cosTheta;
            float by = baseRadius * sinTheta;
            float bz = -halfHeight;
            // The cap's flat normal.
            float nx = 0.0f, ny = 0.0f, nz = -1.0f;
            vertices.push_back(bx);
            vertices.push_back(by);
            vertices.push_back(bz);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
        // Bottom cap center vertex.
        int bottomCenterIndex = static_cast<int>(vertices.size() / 6);
        vertices.push_back(0.0f);  // x
        vertices.push_back(0.0f);  // y
        vertices.push_back(-halfHeight);  // z
        vertices.push_back(0.0f);  // normal x
        vertices.push_back(0.0f);  // normal y
        vertices.push_back(-1.0f); // normal z

        // Bottom cap indices.
        // Note: To have a consistent winding order (counterclockwise when looking from the bottom),
        // we define triangles as (center, next rim vertex, current rim vertex).
        for (int i = 0; i < sectorCount; ++i)
        {
            int current = bottomCapVertexStart + i;
            int next = bottomCapVertexStart + i + 1;
            indices.push_back(bottomCenterIndex);
            indices.push_back(next);
            indices.push_back(current);
        }

        // ========================
        // 3. Generate Top Cap
        // ========================
        int topCapVertexStart = static_cast<int>(vertices.size() / 6);
        for (int i = 0; i <= sectorCount; ++i)
        {
            float theta = i * sectorStep;
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            // Position on the top rim.
            float tx = topRadius * cosTheta;
            float ty = topRadius * sinTheta;
            float tz = halfHeight;
            // The cap's flat normal for the top.
            float nx = 0.0f, ny = 0.0f, nz = 1.0f;
            vertices.push_back(tx);
            vertices.push_back(ty);
            vertices.push_back(tz);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
        // Top cap center vertex.
        int topCenterIndex = static_cast<int>(vertices.size() / 6);
        vertices.push_back(0.0f);  // x
        vertices.push_back(0.0f);  // y
        vertices.push_back(halfHeight);  // z
        vertices.push_back(0.0f);  // normal x
        vertices.push_back(0.0f);  // normal y
        vertices.push_back(1.0f);  // normal z

        // Top cap indices.
        // For the top cap, we want a counterclockwise winding order when viewed from above.
        for (int i = 0; i < sectorCount; ++i)
        {
            int current = topCapVertexStart + i;
            int next = topCapVertexStart + i + 1;
            indices.push_back(topCenterIndex);
            indices.push_back(current);
            indices.push_back(next);
        }

        // Return the total number of vertices (each vertex is 6 floats).
        return static_cast<int>(vertices.size() / 6);
    }

}
