#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Shader.h"

#define MAX_BONE_INFLUENCE 4
namespace isaacObjectLoader
{
    /**
     * @brief a collection of data that represent a single point
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

    struct MeshTexture
    {
        // The id of the texture
        unsigned int id;
        // The type e.g diffuse or specular texture
        std::string type;
        // The path of the texture
        std::string path;
    };

    class Mesh
    {
    public:
        // Mesh Data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<MeshTexture> textures;
        unsigned int VAO;

        // constructor
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures)
        {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            // now that we have all the required data, set the vertex buffers and its attribute pointers.
            SetupMesh();
        }

        // render the mesh
        void Render(Shader &shader);

    private:
        // render data
        unsigned int VBO, EBO;

        // initializes all the buffer objects/arrays
        void SetupMesh();
    };
}