#pragma once

#include "Utility/config.h"
#include "Buffers/IndexBuffer.h"
#include "Buffers/VertexArray.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/VertexBufferLayout.h"
#include "Shaders/Shader.h"
#include "Graphics/Renderer/IRenderable.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"

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

        // match GL attribute type (unsigned int)
        unsigned int m_BoneIDs[MAX_BONE_INFLUENCE] = {0};
        float        m_Weights[MAX_BONE_INFLUENCE] = {0.f};
    };

    class Mesh : public IRenderable
    {
    public:
        Mesh(const std::vector<Vertex>& vertices,
             const std::vector<unsigned int>& indices,
             const std::vector<std::shared_ptr<Texture>>& textures, 
             Material material, const std::string& name);
        
        Mesh(const Mesh&);
        Mesh& operator=(const Mesh&);
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;
        ~Mesh() = default;

        void Render(const Renderer& renderer, 
                    const glm::mat4& view, 
                    const glm::mat4& projection, 
                    Shader* shader = nullptr) override;

        void RenderWithParent(const Renderer& renderer,
                              const glm::mat4& parentModel,
                              const glm::mat4& view,
                              const glm::mat4& projection,
                              Shader* shader = nullptr);

        std::size_t GetID() const override { return 0; } // Mesh does not have an ID
        const std::string& GetName() const override { return m_Name; }
        void SetName(const std::string& newName) override { m_Name = newName; }
        ObjectType GetType() const override { return ObjectType::Imported; }

        glm::vec3& GetPosition() override { return m_Position; }
        glm::vec3& GetRotation() override { return m_Rotation; }
        glm::quat& GetOrientation() override { return m_Orientation; }
        glm::vec3& GetScale() override { return m_Scale; }

        void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }
        void SetRotation(const glm::vec3& newRotation) override { m_Rotation = newRotation; }
        void SetOrientation(const glm::quat& newOrientation) override { m_Orientation = newOrientation; }
        void SetScale(const glm::vec3& newScale) override { m_Scale = newScale; }

        void SetMaterial(Material newMaterial) { m_Material = newMaterial; }
        void SetShininess(float s) { m_Material.Shininess = s; }

        const Material& GetMaterial() const { return m_Material; }
        void SetDiffuseTexture (const std::shared_ptr<Texture>& t) { m_Material.Diffuse  = t; }
        void SetSpecularTexture(const std::shared_ptr<Texture>& t) { m_Material.Specular = t; }
        float GetShininess() const { return m_Material.Shininess; }

        const VertexArray*  GetVertexArray()  const { return m_VertexArray.get(); }
        const VertexBuffer* GetVertexBuffer() const { return m_VertexBuffer.get(); }
        const IndexBuffer*  GetIndexBuffer()  const { return m_IndexBuffer.get(); }
        unsigned int        GetIndexCount()   const { return static_cast<unsigned int>(m_Indices.size()); }
        unsigned int        GetVertexCount()  const { return static_cast<unsigned int>(m_Vertices.size()); }

        // AABB
        const glm::vec3& GetBBoxMin() const { return m_BBoxMin; }
        const glm::vec3& GetBBoxMax() const { return m_BBoxMax; }
    private:
        void SetupMesh();

    private:

        std::vector<Vertex> m_Vertices;
        std::vector<unsigned int> m_Indices;
        std::vector<std::shared_ptr<Texture>> m_Textures;

        Material m_Material;

        std::string m_Name;
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::quat m_Orientation;
        glm::vec3 m_Scale;

        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexArray> m_VertexArray;

        glm::vec3 m_BBoxMin;
        glm::vec3 m_BBoxMax; 

    };
}
