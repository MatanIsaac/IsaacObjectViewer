#pragma once

#include "Utility/config.h"
#include "Buffers/IndexBuffer.h"
#include "Buffers/VertexArray.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/VertexBufferLayout.h"
#include "Shaders/Shader.h"
#include "Texture.h"
#include "Material.h"

#define MAX_BONE_INFLUENCE 4

namespace isaacObjectViewer
{    
    struct Vertex 
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
        int   m_BoneIDs[MAX_BONE_INFLUENCE] = {0};
        float m_Weights[MAX_BONE_INFLUENCE] = {0.f};
    };

    class Mesh 
    {
    public:
        Mesh(const std::vector<Vertex>& vertices,
             const std::vector<unsigned int>& indices,
             const std::vector<std::shared_ptr<Texture>>& textures, 
             Material material);

        void Draw(Shader &shader);
        
        void SetMaterial(Material newMaterial) { m_Material = newMaterial; }
        void SetShininess(float s) { m_Material.Shininess = s; }

        const Material& GetMaterial() const { return m_Material; }
        void SetDiffuseTexture (const std::shared_ptr<Texture>& t) { m_Material.Diffuse  = t; }
        void SetSpecularTexture(const std::shared_ptr<Texture>& t) { m_Material.Specular = t; }
        float GetShininess() const { return m_Material.Shininess; }



        const VertexArray*  GetVertexArray()  const { return m_VertexArray.get(); }
        const VertexBuffer* GetVertexBuffer() const { return m_VertexBuffer.get(); }
        const IndexBuffer*  GetIndexBuffer()  const { return m_IndexBuffer.get(); }
        unsigned int        GetIndexCount()   const { return static_cast<unsigned int>(indices.size()); }

        // AABB
        const glm::vec3& GetBBoxMin() const { return m_BBoxMin; }
        const glm::vec3& GetBBoxMax() const { return m_BBoxMax; }
    private:
        void SetupMesh();

    private:

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<std::shared_ptr<Texture>> textures;
        
        Material m_Material;

        glm::vec3 m_BBoxMin;
        glm::vec3 m_BBoxMax; 
        
        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexArray> m_VertexArray;
    };
}
