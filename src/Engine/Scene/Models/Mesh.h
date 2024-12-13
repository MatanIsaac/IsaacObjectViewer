#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Engine/Graphics/OpenGL/Shaders/Shader.h"
#include "Engine/Graphics/Texture.hpp"

#define MAX_BONE_INFLUENCE 4
namespace isaacGraphicsEngine
{
    /**
     * @brief a collection of data that represent a single point
     


        // Create and bind buffers using smart pointers
        m_VertexArray = std::make_unique<VertexArray>();
        m_VertexBuffer = std::make_unique<VertexBuffer>(MeshVertices, sizeof(MeshVertices));
        m_IndexBuffer = std::make_unique<IndexBuffer>(mesh_indices, MeshIndicesCount);

        VertexBufferLayout vb_layout;
        vb_layout.Push<float>(3); // Positions
        vb_layout.Push<float>(3); // Normals
        vb_layout.Push<float>(2); // TexCoords
        vb_layout.Push<float>(3); // Tangent
        vb_layout.Push<float>(3); // Bitangent
        vb_layout.Push<int>(MAX_BONE_INFLUENCE); // BoneIDs
        vb_layout.Push<float>(MAX_BONE_INFLUENCE); // Weights

        

        const glm::vec3 DEFAULT_POSITION = {1.0f, 1.0f, 1.0f};
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::vec3 m_Scale;

        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexArray> m_VertexArray;
        int m_VertexCount;
        int m_IndicesCount;

     */
    struct Vertex
    {
        // The position of the vertex in 3D space (x, y, z coordinates).
        glm::vec3 Position;
        // Used for lighting calculations, indicating the direction perpendicular to the surface at the vertex.
        glm::vec3 Normal;
        // Used for texture mapping, specifying how textures are applied to the surface.
        glm::vec2 TexCoords;

        // Useful for advanced lighting techniques like normal mapping to
        // provide additional information about the surface's orientation.
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
        // ----------------------------------------------------

        // Used in skeletal animation to specify how a vertex
        // is influenced by different bones in a character rig.
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        float m_Weights[MAX_BONE_INFLUENCE];
        // ----------------------------------------------------
    };

    class Mesh
    {
    public:
        // Mesh Data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        unsigned int VAO;

        // constructor
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

        // render the mesh
        void Render(Shader &shader);

    private:
        // render data
        unsigned int VBO, EBO;

        // initializes all the buffer objects/arrays
        void SetupMesh();
    };
}