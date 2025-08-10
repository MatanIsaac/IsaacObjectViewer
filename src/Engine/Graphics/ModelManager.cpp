#include "ModelManager.h"
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT // gives robust triangulation. Requires C++11
#include "tiny_obj_loader.h"
#include "Utility/Log.hpp"
#include "Mesh.h"
#include "Model.h"
#include "Core/Engine.h"
#include "Graphics/TextureManager.h"
#include <unordered_map>

namespace isaacObjectViewer {

namespace {
// 1×1 solid-color texture for Kd/Ks-only materials
static std::shared_ptr<Texture> makeSolidTexture(unsigned char r,
                                                 unsigned char g,
                                                 unsigned char b,
                                                 TextureType type)
{
    auto tex = std::make_shared<Texture>();
    unsigned char px[4] = { r, g, b, 255 };
    tex->Generate(1, 1, px, GL_RGBA, GL_RGBA, type);
    return tex;
}
static inline unsigned char to8bit(float v)
{
    if (v < 0.f) 
        v = 0.f; 
    if (v > 1.f) 
        v = 1.f;
    return static_cast<unsigned char>(v * 255.f + 0.5f);
}

// Convert tinyobj material → engine Material (load textures if present,
// otherwise synthesize from Kd/Ks colors)
static Material toEngineMaterial(const tinyobj::material_t& m,
                                 const std::string& baseDir)
{
    Material out{};

    // Shininess: map MTL Ns(0..1000) into a sane Blinn-Phong-ish range
    const float Ns = (m.shininess > 0.f ? m.shininess : 32.f);
    out.Shininess = std::max(1.0f, std::min(Ns, 1000.0f) * 0.128f);

    // Diffuse
    if (!m.diffuse_texname.empty()) {
        auto path = (std::filesystem::path(baseDir) / m.diffuse_texname).string();
        out.Diffuse = TextureManager::LoadTexture(path, TextureType::DIFFUSE);
    } else {
        out.Diffuse = makeSolidTexture(to8bit(m.diffuse[0]),
                                       to8bit(m.diffuse[1]),
                                       to8bit(m.diffuse[2]),
                                       TextureType::DIFFUSE);
    }

    // Specular
    if (!m.specular_texname.empty()) {
        auto path = (std::filesystem::path(baseDir) / m.specular_texname).string();
        out.Specular = TextureManager::LoadTexture(path, TextureType::SPECULAR);
    } else {
        const float ks = std::max({m.specular[0], m.specular[1], m.specular[2]});
        if (ks > 0.f) {
            auto s = to8bit(ks);
            out.Specular = makeSolidTexture(s, s, s, TextureType::SPECULAR);
        }
    }
    return out;
}
} // anon

// ------------------------------------------------------------------------

Model* ModelManager::LoadObjModel(const std::string& objFile)
{
    namespace fs = std::filesystem;

    fs::path objPath(objFile);
    if (!fs::exists(objPath)) {
        LOG_ERROR("OBJ not found: {}", objFile);
        return nullptr;
    }

    // Base directory with trailing separator for MTL + textures
    const std::string baseDir = (objPath.parent_path() / "").string();

    tinyobj::ObjReaderConfig cfg;
    cfg.mtl_search_path = baseDir; // <-- important
    cfg.triangulate     = true;

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(objFile, cfg)) 
    {
        if (!reader.Error().empty()) 
            LOG_ERROR("tinyobj: {}", reader.Error());
        return nullptr;
    }
    if (!reader.Warning().empty()) LOG_INFO("tinyobj warn: {}", reader.Warning());

    const auto& attrib    = reader.GetAttrib();
    const auto& shapes    = reader.GetShapes();
    const auto& materials = reader.GetMaterials();

    // Build engine materials array once
    std::vector<Material> engineMats;
    engineMats.reserve(materials.size());
    for (const auto& m : materials) {
        engineMats.emplace_back(toEngineMaterial(m, baseDir));
    }

    auto* model = new Model(objPath.stem().string());

    // For each shape, bucket faces by material id → submeshes (fast & correct)
    for (const auto& shape : shapes)
    {
        // material_id -> list of triangle indices
        struct FaceBucket 
        { 
            std::vector<unsigned> idxs; 
        };
        std::unordered_map<int, FaceBucket> buckets; // -1 means no material
        buckets.reserve(8);

        // De-dup across the whole shape (best perf)
        struct Key 
        { 
            int v, vt, vn; 
        };
        struct KeyHash 
        {
            size_t operator()(const Key& k) const noexcept 
            {
                return (size_t)k.v*73856093u ^ (size_t)k.vt*19349663u ^ (size_t)k.vn*83492791u;
            }
        };
        struct KeyEq 
        { 
            bool operator()(const Key& a, const Key& b) const noexcept 
            {
                return a.v==b.v && a.vt==b.vt && a.vn==b.vn; 
            } 
        };

        std::unordered_map<Key, unsigned, KeyHash, KeyEq> remap;
        remap.reserve(shape.mesh.indices.size());

        std::vector<Vertex> verts;
        verts.reserve(shape.mesh.indices.size());

        auto pushVertex = [&](const tinyobj::index_t& id)->unsigned 
        {
            Key key
            { 
                id.vertex_index, 
                id.texcoord_index, 
                id.normal_index 
            };
            auto it = remap.find(key);
            if (it != remap.end()) 
                return it->second;

            Vertex v{};
            if (id.vertex_index >= 0) 
            {
                v.Position = 
                {
                    attrib.vertices[3*id.vertex_index + 0],
                    attrib.vertices[3*id.vertex_index + 1],
                    attrib.vertices[3*id.vertex_index + 2]
                };
            }
            if (id.normal_index >= 0) 
            {
                v.Normal = 
                {
                    attrib.normals[3*id.normal_index + 0],
                    attrib.normals[3*id.normal_index + 1],
                    attrib.normals[3*id.normal_index + 2]
                };
            }
            if (id.texcoord_index >= 0) 
            {
                v.TexCoords = 
                {
                    attrib.texcoords[2 * id.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * id.texcoord_index + 1] // flip V
                };
            }
            unsigned idx = (unsigned)verts.size();
            verts.push_back(v);
            remap.emplace(key, idx);
            return idx;
        };

        size_t off = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f)
        {
            const int fv = shape.mesh.num_face_vertices[f]; // triangulated => 3
            if (fv < 3) 
            { 
                off += fv; 
                continue; 
            }

            const int matID = (f < shape.mesh.material_ids.size())
                                ? shape.mesh.material_ids[f] : -1;

            unsigned i0 = pushVertex(shape.mesh.indices[off + 0]);
            for (int k = 1; k + 1 < fv; ++k) 
            {
                unsigned i1 = pushVertex(shape.mesh.indices[off + k + 0]);
                unsigned i2 = pushVertex(shape.mesh.indices[off + k + 1]);
                buckets[matID].idxs.insert(buckets[matID].idxs.end(), { i0, i1, i2 });
            }
            off += fv;
        }

        // Emit one Mesh per material bucket
        for (auto& [matID, bucket] : buckets)
        {
            // Pick material (defaults if out of range / -1)
            Material mat{};
            if (matID >= 0 && matID < (int)engineMats.size())
                mat = engineMats[matID];

            Mesh mesh(verts, bucket.idxs, /*textures=*/{}, mat);
            model->AddMesh(std::move(mesh));
        }
    }

    Engine::GetInstance()->AddSceneObject(model);
    return model;
}

} // namespace isaacObjectViewer
