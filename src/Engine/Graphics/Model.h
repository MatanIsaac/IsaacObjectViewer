/** @file Model.h
 *  @brief Header file for the Model class.
 *  This class is responsible for managing and rendering 3D model data.
 *  It encapsulates multiple meshes and their associated properties.
 *  The Model class provides methods for loading, rendering, and manipulating 3D models.
 */

#pragma once
#include "Utility/config.h"

#include "Graphics/Ray.h"
#include "Graphics/Mesh.h"          
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Shader/Shader.h"
#include "IObject.h"

namespace isaacObjectViewer
{

    /// @brief Enumeration for different model file types.
    enum class ModelFileType
    {
        Unknown,
        OBJ,
        FBX,
    };

    class Model : public IObject
    {
    public:
        /// @brief Constructs a Model object.
        /// @param meshes The meshes that make up the model.
        /// @param name The name of the model.
        explicit Model(const std::vector<Mesh>& meshes, const std::string& name);

        
        /// @brief Gets the ID of the model.
        /// @return The ID of the model.
        std::size_t          GetID()   const override { return m_ID; }

        /// @brief Gets the name of the model.
        /// @return The name of the model.
        const std::string&   GetName() const override { return m_Name; }

        /// @brief Sets the name of the model.
        /// @param n The new name of the model.
        void                 SetName(const std::string& n) override { m_Name = n; }

        /// @brief Gets the type of the model.
        /// @return The type of the model.
        ObjectType           GetType() const override { return ObjectType::Imported; } // Imported asset

        /// @brief Gets the file type of the model.
        /// @return The file type of the model.
        ModelFileType        GetFileType() const { return m_FileType; }

        /// @brief Gets the position of the model.
        /// @return The position of the model.
        glm::vec3&          GetPosition()    override { return m_Position;    }

        /// @brief Gets the rotation of the model.
        /// @return The rotation of the model.
        glm::vec3&          GetRotation()    override { return m_Rotation;    }

        /// @brief Gets the orientation of the model.
        /// @return The orientation of the model.
        glm::quat&          GetOrientation() override { return m_Orientation; }

        /// @brief Gets the scale of the model.
        /// @return The scale of the model.
        glm::vec3&          GetScale()       override { return m_Scale;       }

        /// @brief Gets the color of the model.
        /// @return The color of the model.
        glm::vec3&          GetColor()       override { return m_Color;       }

        /// @brief Gets the use material flag of the model.
        /// @return The use material flag of the model.
        bool&               GetUseMaterial() override { return m_UseMaterial; }

        /// @brief Gets the shininess of the model.
        /// @return The shininess of the model.
        float&              GetShininess()   override { return m_Shininess;   }

        /// @brief Sets the position of the model.
        /// @param p The new position of the model.
        void    SetPosition   (const glm::vec3& p) override { m_Position   = p; }

        /// @brief Sets the rotation of the model.
        /// @param r The new rotation of the model.
        void    SetRotation   (const glm::vec3& r) override { m_Rotation   = r; m_Orientation = glm::quat(glm::radians(r)); }

        /// @brief Sets the orientation of the model.
        /// @param q The new orientation of the model.
        void    SetOrientation(const glm::quat& q) override { m_Orientation= q; m_Rotation = glm::degrees(glm::eulerAngles(q)); }

        /// @brief Sets the scale of the model.
        /// @param s The new scale of the model.
        void    SetScale      (const glm::vec3& s) override { m_Scale      = s; }

        /// @brief Sets the color of the model.
        /// @param c The new color of the model.
        void    SetColor      (const glm::vec3& c) override { m_Color      = c; }

        /// @brief Sets the use material flag of the model.
        /// @param useMaterial The new use material flag of the model.
        void    SetUseMaterial(bool useMaterial)   override { m_UseMaterial = useMaterial; }

        /// @brief Sets the diffuse texture of the model.
        /// @param texture The new diffuse texture of the model.
        void SetDiffuseTexture (const std::shared_ptr<Texture>& texture) override;

        /// @brief Sets the specular texture of the model.
        /// @param texture The new specular texture of the model.
        void SetSpecularTexture(const std::shared_ptr<Texture>& texture) override;

        /// @brief Renders the model.
        /// @param renderer The renderer to use for rendering the model.
        /// @param view The view matrix.
        /// @param projection The projection matrix.
        /// @param shader The shader to use for rendering the model.
        void Render(const Renderer& renderer,
                    const glm::mat4& view,
                    const glm::mat4& projection,
                    Shader* shader) override;

        /// @brief Gets the vertex array of the model.
        /// @return The vertex array of the model.
        const VertexArray&  GetVertexArray () const override { return *m_Meshes.front().GetVertexArray(); }

        /// @brief Gets the vertex buffer of the model.
        /// @return The vertex buffer of the model.
        const VertexBuffer& GetVertexBuffer() const override { return *m_Meshes.front().GetVertexBuffer(); }

        /// @brief Gets the index buffer of the model.
        /// @return The index buffer of the model.
        const IndexBuffer&  GetIndexBuffer () const override { return *m_Meshes.front().GetIndexBuffer(); }

        /// @brief Gets the index count of the model.
        /// @return The index count of the model.
        unsigned int        GetIndexCount  () const override { return  m_Meshes.front().GetIndexCount(); }
        
        /// @brief Gets the vertex count of the model.
        /// @return The vertex count of the model.
        unsigned int        GetVertexCount() const override { return  m_Meshes.front().GetVertexCount(); }

        /// @brief Checks if a ray intersects with the model.
        /// @param ray The ray to check for intersection.
        /// @param outDist The distance to the intersection point, if any.
        /// @return True if the ray intersects the model, false otherwise.
        bool IntersectRay(const Ray& ray, float* outDist) override;

        /// @brief Adds a mesh to the model.
        /// @param mesh The mesh to add.
        void AddMesh(Mesh&& mesh) { m_Meshes.emplace_back(std::move(mesh)); }

        /// @brief Generates a unique ID for the model.
        /// @return The unique ID for the model.
        std::size_t GenerateUniqueID() override;                 // (kept for completeness)

    private:
        std::size_t         m_ID;
        std::string         m_Name;
        ModelFileType       m_FileType;

        glm::vec3           m_Position;
        glm::vec3           m_Rotation;
        glm::quat           m_Orientation;
        glm::vec3           m_Scale;
        glm::vec3           m_Color;
        bool                m_UseMaterial;
        float               m_Shininess;
        
        std::vector<Mesh>   m_Meshes;

        /* Cached AABB for fast pick-testing */
        glm::vec3           m_BBoxMin {  std::numeric_limits<float>::max() };
        glm::vec3           m_BBoxMax { -std::numeric_limits<float>::max() };

        /// @brief Recomputes the bounding box of the model.
        void RecomputeBoundingBox();
    };
}
