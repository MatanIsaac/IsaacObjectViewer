#include "ModelManager.h"
#include "Utility/Log.hpp"
#include "Mesh.h"
#include "Model.h"
#include "Core/Engine.h"
#include "Graphics/TextureManager.h"
#include <filesystem>

namespace isaacObjectViewer 
{
    // --- tiny helpers --------------------------------------------------------
    static std::shared_ptr<Texture> makeSolidTexture(unsigned char r, unsigned char g,
                                                     unsigned char b, TextureType type)
    {
        auto tex = std::make_shared<Texture>();
        unsigned char px[4] = { r, g, b, 255 };
        tex->Generate(1, 1, px, GL_RGBA, GL_RGBA, type);
        return tex;
    }

    static inline unsigned char to8bit(float v)
    {
        v = (v < 0.f) ? 0.f : (v > 1.f ? 1.f : v);
        return static_cast<unsigned char>(v * 255.f + 0.5f);
    }

    std::string ModelManager::canonicalUnderBase(const std::filesystem::path& p) const
    {
        std::error_code ec;
        auto full = (p.is_absolute() ? p : (m_BaseDir / p));
        auto can  = std::filesystem::weakly_canonical(full, ec);
        return (ec ? full : can).string();
    }

    // Convert Assimp material → engine Material (textures or synthesized colors)
    static Material toEngineMaterial(const aiMaterial* mat,
                                     const std::filesystem::path& baseDir)
    {
        Material out{};
        float shininess = 32.0f;
        mat->Get(AI_MATKEY_SHININESS, shininess);
        out.Shininess = std::max(1.0f, std::min(shininess, 1000.0f) * 0.128f);

        aiColor3D kd(1.0f, 1.0f, 1.0f);
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, kd);

        aiString texPath;
        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0 &&
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
        {
            auto path = (baseDir / texPath.C_Str()).string();
            out.Diffuse = TextureManager::LoadTexture(path, TextureType::DIFFUSE);
        }
        else
        {
            out.Diffuse = makeSolidTexture(to8bit(kd.r), to8bit(kd.g), to8bit(kd.b), TextureType::DIFFUSE);
        }

        aiColor3D ks(0.0f, 0.0f, 0.0f);
        mat->Get(AI_MATKEY_COLOR_SPECULAR, ks);

        if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0 &&
            mat->GetTexture(aiTextureType_SPECULAR, 0, &texPath) == AI_SUCCESS)
        {
            auto path = (baseDir / texPath.C_Str()).string();
            out.Specular = TextureManager::LoadTexture(path, TextureType::SPECULAR);
        }
        else
        {
            float s = std::max({ks.r, ks.g, ks.b});
            if (s > 0.f)
            {
                auto v = to8bit(s);
                out.Specular = makeSolidTexture(v, v, v, TextureType::SPECULAR);
            }
        }
        return out;
    }

    // ------------------------------------------------------------------------

    Model* ModelManager::LoadModel(const std::string &path)
    {
        m_Meshes.clear();

        // robust base dir
        std::filesystem::path p(path);
        m_BaseDir = p.parent_path();

        Assimp::Importer import;

        // IMPORTANT: add safe flags for normals/tangents
        const unsigned int flags =
            aiProcess_Triangulate
          | aiProcess_FlipUVs
          | aiProcess_JoinIdenticalVertices
          | aiProcess_ImproveCacheLocality
          | aiProcess_LimitBoneWeights
          | aiProcess_GenSmoothNormals
          | aiProcess_CalcTangentSpace;

        const aiScene *scene = import.ReadFile(path, flags);

        if(!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) 
        {
            LOG_ERROR("ERROR::ASSIMP::{}", import.GetErrorString());
            return nullptr;
        }

        ProcessNode(scene->mRootNode, scene);

        std::string modelName = p.stem().string();
        auto model = new Model(m_Meshes, modelName);
        
        if (path.ends_with(".fbx") || path.ends_with(".dae"))
        {
            // FBX/DAE models are Z-up, convert to Y-up
            model->SetOrientation(glm::quat(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f)));
        }

        return model;
    }

    void ModelManager::ProcessNode(aiNode *node, const aiScene *scene)
    {
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
            Mesh m = ProcessMesh(mesh, scene);
            if (m.GetIndexCount() > 0 || m.GetVertexCount() > 0) // skip empty
                m_Meshes.push_back(std::move(m));
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i], scene);
    }  
    
    Mesh ModelManager::ProcessMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::string meshName = mesh->mName.C_Str();
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        vertices.reserve(mesh->mNumVertices);
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex v{};

            // positions
            v.Position.x = mesh->mVertices[i].x;
            v.Position.y = mesh->mVertices[i].y;
            v.Position.z = mesh->mVertices[i].z;

            // normals (assimp flag will generate if missing)
            if (mesh->HasNormals())
            {
                v.Normal.x = mesh->mNormals[i].x;
                v.Normal.y = mesh->mNormals[i].y;
                v.Normal.z = mesh->mNormals[i].z;
            }
            else
            {
                v.Normal = glm::vec3(0.0f);
            }

            // UVs + tangents/bitangents
            if (mesh->mTextureCoords[0])
            {
                v.TexCoords.x = mesh->mTextureCoords[0][i].x; 
                v.TexCoords.y = mesh->mTextureCoords[0][i].y;

                if (mesh->HasTangentsAndBitangents())
                {
                    v.Tangent.x   = mesh->mTangents[i].x;
                    v.Tangent.y   = mesh->mTangents[i].y;
                    v.Tangent.z   = mesh->mTangents[i].z;

                    v.Bitangent.x = mesh->mBitangents[i].x;
                    v.Bitangent.y = mesh->mBitangents[i].y;
                    v.Bitangent.z = mesh->mBitangents[i].z;
                }
                else
                {
                    v.Tangent   = glm::vec3(0.0f);
                    v.Bitangent = glm::vec3(0.0f);
                }
            }
            else
            {
                v.TexCoords = glm::vec2(0.0f);
                v.Tangent   = glm::vec3(0.0f);
                v.Bitangent = glm::vec3(0.0f);
            }

            vertices.push_back(v);
        }

        // indices
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // materials → textures
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        auto diffuse  = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
        auto specular = LoadMaterialTextures(material, aiTextureType_SPECULAR, "specular");

        // normals: prefer NORMALS, then fallback to HEIGHT or DISPLACEMENT (quirky exporters)
        auto normals  = LoadMaterialTextures(material, aiTextureType_NORMALS, "normal");
        if (normals.empty())
            normals = LoadMaterialTextures(material, aiTextureType_HEIGHT, "normal(height)");
        if (normals.empty())
            normals = LoadMaterialTextures(material, aiTextureType_DISPLACEMENT, "normal(disp)");

        // height/displacement (optional)
        auto height   = LoadMaterialTextures(material, aiTextureType_HEIGHT, "height");
        if (height.empty())
            height = LoadMaterialTextures(material, aiTextureType_DISPLACEMENT, "height(disp)");

        // combine
        std::vector<std::shared_ptr<Texture>> textures;
        textures.reserve(diffuse.size() + specular.size() + normals.size() + height.size());
        textures.insert(textures.end(), diffuse.begin(),  diffuse.end());
        textures.insert(textures.end(), specular.begin(), specular.end());
        textures.insert(textures.end(), normals.begin(),  normals.end());
        textures.insert(textures.end(), height.begin(),   height.end());

        // engine Material (includes solid-color fallbacks)
        Material engineMaterial = toEngineMaterial(material, m_BaseDir);

        //LOG_INFO("ModelManager::ProcessMesh - Mesh: {}, V:{} I:{} Tex:{}",
        //         meshName, vertices.size(), indices.size(), textures.size());

        return Mesh(vertices, indices, textures, engineMaterial, meshName);
    }

    std::vector<std::shared_ptr<Texture>> ModelManager::LoadMaterialTextures(
        aiMaterial *mat, aiTextureType type, const char* /*debugTypeName*/)
    {
        std::vector<std::shared_ptr<Texture>> out;

        const unsigned int count = mat->GetTextureCount(type);
        for(unsigned int i = 0; i < count; i++)
        {
            aiString rel;
            if (mat->GetTexture(type, i, &rel) != AI_SUCCESS) continue;

            // canonical full path (handles relative vs absolute and / vs \)
            const std::string canon = canonicalUnderBase(rel.C_Str());

            // cache check
            if (auto it = m_TextureCache.find(canon); it != m_TextureCache.end())
            {
                if (auto sp = it->second.lock())
                {
                    out.push_back(sp);
                    continue;
                }
            }

            // decide TextureType
            TextureType ttype = TextureType::DIFFUSE;
            switch (type)
            {
                case aiTextureType_DIFFUSE:       ttype = TextureType::DIFFUSE;       break;
                case aiTextureType_SPECULAR:      ttype = TextureType::SPECULAR;      break;
                case aiTextureType_NORMALS:       ttype = TextureType::NORMAL;        break;
                case aiTextureType_HEIGHT:        ttype = TextureType::HEIGHT;        break;
                case aiTextureType_DISPLACEMENT:  ttype = TextureType::HEIGHT;        break;
                default:                          ttype = TextureType::DIFFUSE;       break;
            }

            auto tex = TextureManager::LoadTexture(canon, ttype);
            if (tex)
            {
                m_TextureCache[canon] = tex;   // store weak_ptr
                out.push_back(tex);
            }
        }
        return out;
    }
}
