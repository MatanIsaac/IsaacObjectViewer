/**
 * @file Mesh.h
 * @brief Header file for the Mesh class.
 * This class is responsible for managing and rendering 3D mesh data.
 */

#pragma once

#include "Utility/config.h"
#include "Buffers/IndexBuffer.h"
#include "Buffers/VertexArray.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/VertexBufferLayout.h"
#include "Shader/Shader.h"
#include "Graphics/Renderer/IRenderable.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"

#define MAX_BONE_INFLUENCE 4

namespace isaacObjectViewer
{    
    /// @brief Represents a single vertex in 3D space.
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
        /// @brief Constructs a Mesh object.
        /// @param vertices The vertices of the mesh.
        /// @param indices The indices for the mesh.
        /// @param textures The textures used by the mesh.
        /// @param material The material properties of the mesh.
        /// @param name The name of the mesh.
        Mesh(const std::vector<Vertex>& vertices,
             const std::vector<unsigned int>& indices,
             const std::vector<std::shared_ptr<Texture>>& textures, 
             Material material, const std::string& name);

        /// @brief Copy constructor.
        Mesh(const Mesh&);
        
        /// @brief Copy assignment operator.
        Mesh& operator=(const Mesh&);
        
        /// @brief Move constructor.
        Mesh(Mesh&& other) noexcept;
        
        /// @brief Move assignment operator.
        Mesh& operator=(Mesh&& other) noexcept;
        
        /// @brief Destructor.
        ~Mesh() = default;

        /// @brief Renders the mesh.
        void Render(const Renderer& renderer, 
                    const glm::mat4& view, 
                    const glm::mat4& projection, 
                    Shader* shader = nullptr) override;

        /// @brief Renders the mesh with a parent model transformation.
        void RenderWithParent(const Renderer& renderer,
                              const glm::mat4& parentModel,
                              const glm::mat4& view,
                              const glm::mat4& projection,
                              Shader* shader = nullptr,
                              bool useMaterial = false,
                              const glm::vec3& objectColor = glm::vec3(DEFAULT_COLOR));

        /// @brief Gets the ID of the mesh.
        /// @return The ID of the mesh.
        std::size_t GetID() const override { return 0; } // Mesh does not use an ID currently

        /// @brief Gets the name of the mesh.
        /// @return The name of the mesh.
        const std::string& GetName() const override { return m_Name; }

        /// @brief Sets the name of the mesh.
        /// @param newName The new name for the mesh.
        void SetName(const std::string& newName) override { m_Name = newName; }

        /// @brief Gets the type of the mesh.
        /// @return The type of the mesh.
        ObjectType GetType() const override { return ObjectType::Imported; }

        /// @brief Gets the position of the mesh.
        /// @return The position of the mesh.
        glm::vec3& GetPosition() override { return m_Position; }

        /// @brief Gets the rotation of the mesh.
        /// @return The rotation of the mesh.
        glm::vec3& GetRotation() override { return m_Rotation; }

        /// @brief Gets the orientation of the mesh.
        /// @return The orientation of the mesh.
        glm::quat& GetOrientation() override { return m_Orientation; }

        /// @brief Gets the scale of the mesh.
        /// @return The scale of the mesh.
        glm::vec3& GetScale() override { return m_Scale; }

        /// @brief Gets the color of the mesh.
        /// @return The color of the mesh.
        glm::vec3& GetColor() override { return m_Color; }

        /// @brief Gets the use material flag of the mesh.
        /// @return The use material flag of the mesh.
        bool& GetUseMaterial() override { return m_UseMaterial; }

        /// @brief Sets the position of the mesh.
        /// @param newPosition The new position for the mesh.
        void SetPosition(const glm::vec3& newPosition) override { m_Position = newPosition; }

        /// @brief Sets the rotation of the mesh.
        /// @param newRotation The new rotation for the mesh.
        void SetRotation(const glm::vec3& newRotation) override { m_Rotation = newRotation; }

        /// @brief Sets the orientation of the mesh.
        /// @param newOrientation The new orientation for the mesh.
        void SetOrientation(const glm::quat& newOrientation) override { m_Orientation = newOrientation; }

        /// @brief Sets the scale of the mesh.
        /// @param newScale The new scale for the mesh.
        void SetScale(const glm::vec3& newScale) override { m_Scale = newScale; }

        /// @brief Sets the color of the mesh.
        /// @param newColor The new color for the mesh.
        void SetColor(const glm::vec3& newColor) override { m_Color = newColor; }

        /// @brief Sets the use material flag of the mesh.
        /// @param use The new use material flag for the mesh.
        void SetUseMaterial(bool use) override { m_UseMaterial = use; }

        /// @brief Sets the material of the mesh.
        /// @param newMaterial The new material for the mesh.
        void SetMaterial(Material newMaterial) { m_Material = newMaterial; }

        /// @brief Sets the shininess of the material.
        /// @param newShininess The new shininess value for the material.
        void SetShininess(float newShininess) { m_Material.Shininess = newShininess; }

        /// @brief Gets the material of the mesh.
        /// @return The material of the mesh.
        const Material& GetMaterial() const { return m_Material; }

        /// @brief Sets the diffuse texture of the material.
        /// @param t The new diffuse texture for the material.
        void SetDiffuseTexture (const std::shared_ptr<Texture>& t) { m_Material.Diffuse  = t; }

        /// @brief Sets the specular texture of the material.
        /// @param t The new specular texture for the material.
        void SetSpecularTexture(const std::shared_ptr<Texture>& t) { m_Material.Specular = t; }

        /// @brief Gets the shininess of the material.
        /// @return The shininess of the material.
        float GetShininess() const { return m_Material.Shininess; }

        /// @brief Gets the vertex array of the mesh.
        /// @return The vertex array of the mesh.
        const VertexArray*  GetVertexArray()  const { return m_VertexArray.get(); }

        /// @brief Gets the vertex buffer of the mesh.
        /// @return The vertex buffer of the mesh.
        const VertexBuffer* GetVertexBuffer() const { return m_VertexBuffer.get(); }
        
        /// @brief Gets the index buffer of the mesh.
        /// @return The index buffer of the mesh.
        const IndexBuffer*  GetIndexBuffer()  const { return m_IndexBuffer.get(); }

        /// @brief Gets the index count of the mesh.
        /// @return The index count of the mesh.
        unsigned int        GetIndexCount()   const { return static_cast<unsigned int>(m_Indices.size()); }

        /// @brief Gets the vertex count of the mesh.
        /// @return The vertex count of the mesh.
        unsigned int        GetVertexCount()  const { return static_cast<unsigned int>(m_Vertices.size()); }

        /// @brief Gets the minimum bounding box of the mesh.
        /// @return The minimum bounding box of the mesh.
        const glm::vec3& GetBBoxMin() const { return m_BBoxMin; }
        /// @brief Gets the maximum bounding box of the mesh.
        /// @return The maximum bounding box of the mesh.
        const glm::vec3& GetBBoxMax() const { return m_BBoxMax; }
    private:
        /// @brief Sets up the mesh.
        /// Creating the vertex array, vertex buffer, and index buffer.
        void SetupMesh();

    private:

        std::vector<Vertex> m_Vertices;
        std::vector<unsigned int> m_Indices;
        std::vector<std::shared_ptr<Texture>> m_Textures;

        
        std::string m_Name;
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::quat m_Orientation;
        glm::vec3 m_Scale;
        glm::vec3 m_Color;
        
        bool m_UseMaterial;
        
        Material m_Material;

        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<VertexBuffer> m_VertexBuffer;
        std::unique_ptr<VertexArray> m_VertexArray;

        glm::vec3 m_BBoxMin;
        glm::vec3 m_BBoxMax; 

    };
}
