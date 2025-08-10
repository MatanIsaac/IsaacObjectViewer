#include "Model.h"

namespace isaacObjectViewer
{
    static std::size_t s_NextModelID = 0;

    Model::Model(const std::string& name)
        : m_ID(++s_NextModelID), m_Name(name)
    {}

    void Model::SetDiffuseTexture(const std::shared_ptr<Texture>& tex)
    {
        for (auto& m : m_Meshes) m.SetDiffuseTexture(tex);
    }

    void Model::SetSpecularTexture(const std::shared_ptr<Texture>& tex)
    {
        for (auto& m : m_Meshes) m.SetSpecularTexture(tex);
    }

    void Model::Render(const Renderer& renderer,
                    const glm::mat4& view,
                    const glm::mat4& proj,
                    Shader* shader)
    {
        if (!shader || m_Meshes.empty()) return;

        shader->Bind();
        shader->setMat4("model", GetModelMatrix());
        shader->setMat4("view",  view);
        shader->setMat4("projection", proj);

        for (auto& m : m_Meshes) 
        {
            m.SetShininess(m_Shininess);
            m.Draw(*shader);
        }
    }

    /* Very coarse: test ray vs model AABB first */
    bool Model::IntersectRay(const Ray& ray, float* outDist)
    {
        RecomputeBoundingBox();                    // cheap enough
        float hitDist;
        if (!RayIntersectsAABB(ray, m_BBoxMin, m_BBoxMax, &hitDist))
            return false;

        /* Optional: iterate meshes for finer test here */

        if (outDist) *outDist = hitDist;
        return true;
    }

    void Model::RecomputeBoundingBox()
    {
        if (m_Meshes.empty()) return;
        m_BBoxMin =  glm::vec3( std::numeric_limits<float>::max());
        m_BBoxMax = -glm::vec3( std::numeric_limits<float>::max());

        for (const auto& mesh : m_Meshes)
        {
            m_BBoxMin = glm::min(m_BBoxMin, mesh.GetBBoxMin());
            m_BBoxMax = glm::max(m_BBoxMax, mesh.GetBBoxMax());
        }

        glm::mat4 M = GetModelMatrix();
        m_BBoxMin = glm::vec3(M * glm::vec4(m_BBoxMin, 1.0));
        m_BBoxMax = glm::vec3(M * glm::vec4(m_BBoxMax, 1.0));
    }

    /* ---------------------------------------------------------- */
    std::size_t Model::GenerateUniqueID()
    {
        return ++s_NextModelID;
    }
}