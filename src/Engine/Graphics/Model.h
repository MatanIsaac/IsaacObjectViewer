#pragma once
#include "Utility/config.h"

#include "Core/Ray.h"
#include "Graphics/Mesh.h"          
#include "Graphics/Renderer/Renderer.h"
#include "Graphics/Shaders/Shader.h"
#include "ISceneObject.h"

namespace isaacObjectViewer
{

    enum class ModelFileType
    {
        Unknown,
        OBJ,
        FBX,
    };

    class Model : public ISceneObject
    {
    public:
        explicit Model(const std::vector<Mesh>& meshes, const std::string& name);

        /* ---------- SceneObject interface ---------- */
        std::size_t          GetID()   const override { return m_ID; }
        const std::string&   GetName() const override { return m_Name; }
        void                 SetName(const std::string& n) override { m_Name = n; }

        ObjectType           GetType() const override { return ObjectType::Imported; } // Imported asset
        ModelFileType        GetFileType() const { return m_FileType; }

        glm::vec3&           GetPosition()    override { return m_Position;    }
        glm::vec3&           GetRotation()    override { return m_Rotation;    }
        glm::quat&           GetOrientation() override { return m_Orientation; }
        glm::vec3&           GetScale()       override { return m_Scale;       }
        float&               GetShininess()   override { return m_Shininess;   }

        void  SetPosition   (const glm::vec3& p) override { m_Position   = p; }
        void  SetRotation   (const glm::vec3& r) override { m_Rotation   = r; m_Orientation = glm::quat(glm::radians(r)); }
        void  SetOrientation(const glm::quat& q) override { m_Orientation= q; m_Rotation = glm::degrees(glm::eulerAngles(q)); }
        void  SetScale      (const glm::vec3& s) override { m_Scale      = s; }

        /* Materials are set per-mesh – keep empty */
        void SetDiffuseTexture (const std::shared_ptr<Texture>&) override;
        void SetSpecularTexture(const std::shared_ptr<Texture>&) override;

        /* ---------- Rendering ---------- */
        void Render(const Renderer& renderer,
                    const glm::mat4& view,
                    const glm::mat4& projection,
                    Shader* shader) override;

        /* ---------- Geometry access required by gizmos / ray-picking ---------- */
        const VertexArray&  GetVertexArray () const override { return *m_Meshes.front().GetVertexArray(); }
        const VertexBuffer& GetVertexBuffer() const override { return *m_Meshes.front().GetVertexBuffer(); }
        const IndexBuffer&  GetIndexBuffer () const override { return *m_Meshes.front().GetIndexBuffer(); }
        unsigned int        GetIndexCount  () const override { return  m_Meshes.front().GetIndexCount(); }
        unsigned int        GetVertexCount() const override { return  m_Meshes.front().GetVertexCount(); }
        
        bool IntersectRay(const Ray& ray, float* outDist) override;

        /* ---------- Loading ---------- */
        void AddMesh(Mesh&& mesh) { m_Meshes.emplace_back(std::move(mesh)); }

        std::size_t GenerateUniqueID() override;                 // (kept for completeness)

    private:
        std::size_t         m_ID;
        std::string         m_Name;
        ModelFileType       m_FileType;

        glm::vec3           m_Position;
        glm::vec3           m_Rotation;
        glm::quat           m_Orientation;
        glm::vec3           m_Scale;
        float               m_Shininess;

        // <-- one asset == many sub-meshes
        std::vector<Mesh>   m_Meshes;

        /* Cached AABB for fast pick-testing */
        glm::vec3           m_BBoxMin {  std::numeric_limits<float>::max() };
        glm::vec3           m_BBoxMax { -std::numeric_limits<float>::max() };

        void RecomputeBoundingBox();
    };
}
