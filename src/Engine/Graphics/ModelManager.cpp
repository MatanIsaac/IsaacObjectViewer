#include "ModelManager.h"
#include "Utility/Log.hpp"
#include "Mesh.h"
#include "Model.h"
#include "Core/Engine.h"
#include "Graphics/TextureManager.h"
#include <filesystem>
#include <functional>
#include <exception>

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

    static const aiTexture* getEmbedded(const aiScene* scene, const aiString& path) 
    {
        return (path.length > 0 && path.C_Str()[0] == '*') ? scene->GetEmbeddedTexture(path.C_Str()) : nullptr;
    }

    // --- unified tiny loader (handles embedded + sRGB) --------------------------
    static std::shared_ptr<Texture> loadMaybeEmbedded(const aiScene* scene,
                                                    const std::filesystem::path& baseDir,
                                                    const std::string& path,
                                                    const aiString& texPath,
                                                    TextureType ttype,
                                                    bool srgb)
    {
        auto baseName = std::filesystem::path(path).stem().string() + "_";
        std::filesystem::path ext = std::filesystem::path(texPath.C_Str()).extension();
        
        switch (ttype) 
        {
            case TextureType::DIFFUSE:      
                baseName += "diffuse";
                break;
            case TextureType::SPECULAR:     
                baseName += "specular"; 
                break;
            case TextureType::NORMAL:     
                baseName += "normal"; 
                break;
            default:                  
                LOG_WARN("Unknown texture type: {}", baseDir.string() + "\\textures\\" + baseName + ext.string());
                break;
        }
        
#if defined(_WIN32)
        auto finalPath = baseDir.string() + "\\textures\\" + baseName + ext.string();
#elif defined(__linux__) || defined(__APPLE__)
        auto finalPath = baseDir.string() + "/textures/" + baseName + ext.string();
#endif
        if (const aiTexture* emb = getEmbedded(scene, texPath)) 
        {
            // Compressed (PNG/JPG/etc.) case: mHeight == 0
            if (emb->mHeight == 0) 
            {
                const unsigned char* bytes = reinterpret_cast<const unsigned char*>(emb->pcData);
                size_t sz = static_cast<size_t>(emb->mWidth);
                return TextureManager::LoadTextureFromMemory(bytes, sz, ttype, srgb, finalPath.c_str());
            } 
            else 
            {
                // Raw pixel data; you can add a fallback if you want later
                LOG_WARN("Embedded raw pixel texture not handled yet: {}", finalPath.c_str());
                return nullptr;
            }
        }

        return TextureManager::LoadTexture(finalPath, ttype);
    }

    // --- Convert Assimp material → engine Material ------------------------------
    static Material toEngineMaterial(const aiMaterial* mat,
                                    const aiScene* scene,
                                    const std::filesystem::path& baseDir,
                                    const std::string& path)
    {
        Material out{};

        // Shininess: clamp to something sane; do NOT scale down
        float shininess = 32.0f;
        mat->Get(AI_MATKEY_SHININESS, shininess);
        if (!(shininess > 0.0f && std::isfinite(shininess))) 
            shininess = 32.0f;
        out.Shininess = std::clamp(shininess, 1.0f, 256.0f);

        aiString texPath;

        // Diffuse (sRGB)
        aiColor3D kd(1.0f, 1.0f, 1.0f);
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, kd);

        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0 &&
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
        {
            out.Diffuse = loadMaybeEmbedded(scene, baseDir, path, texPath, TextureType::DIFFUSE, /*srgb=*/true);
        }
        if (!out.Diffuse) 
        {
            out.Diffuse = makeSolidTexture(to8bit(kd.r), to8bit(kd.g), to8bit(kd.b), TextureType::DIFFUSE);
        }

        // Normal map: try NORMALS -> HEIGHT -> DISPLACEMENT (linear)
        auto loadNormal = [&](aiTextureType t) -> std::shared_ptr<Texture> 
        {
            if (mat->GetTextureCount(t) == 0) 
            {
                LOG_WARN("No normal map texture found");
                return nullptr;
            }
            if (mat->GetTexture(t, 0, &texPath) != AI_SUCCESS) 
            {
                LOG_WARN("Failed to load normal map texture: {}", texPath.C_Str());
                return nullptr;
            }
            return loadMaybeEmbedded(scene, baseDir, path, texPath, TextureType::NORMAL, /*srgb=*/false);
        };

        out.Normal = loadNormal(aiTextureType_NORMALS);
        
        // NOTE: no fake normal map here; shader should disable tangent-space normal mapping if absent

        // Specular: map if present, else constant dielectric 0.04 (linear)
        aiColor3D ks(0,0,0);
        mat->Get(AI_MATKEY_COLOR_SPECULAR, ks);

        if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0 &&
            mat->GetTexture(aiTextureType_SPECULAR, 0, &texPath) == AI_SUCCESS)
        {
            out.Specular = loadMaybeEmbedded(scene, baseDir, path, texPath, TextureType::SPECULAR, /*srgb=*/false);
        }
        if (!out.Specular) 
        {
            float s = std::max({ks.r, ks.g, ks.b});
            unsigned char v = to8bit(s > 0.0f ? s : 0.04f);
            out.Specular = makeSolidTexture(v, v, v, TextureType::SPECULAR);
        }

        return out;
    }

    // ---------------------------------------------------------------------------

    Model* ModelManager::LoadModel(const std::string& path)
    {
        m_Meshes.clear();

        std::filesystem::path p(path);
        m_BaseDir = p.parent_path();

        Assimp::Importer import;

        const unsigned int flags = aiProcess_Triangulate
                                    | aiProcess_FlipUVs
                                    | aiProcess_JoinIdenticalVertices
                                    | aiProcess_ImproveCacheLocality
                                    | aiProcess_LimitBoneWeights
                                    | aiProcess_GenSmoothNormals
                                    | aiProcess_CalcTangentSpace;

        const aiScene* scene = import.ReadFile(path, flags);
        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) 
        {
            LOG_ERROR("ERROR::ASSIMP::{}", import.GetErrorString());
            return nullptr;
        }

        ProcessNode(scene->mRootNode, scene, path);

        std::string modelName = p.stem().string();
        auto model = new Model(m_Meshes, modelName);

        if (path.ends_with(".fbx") || path.ends_with(".dae")) 
        {
            model->SetOrientation(glm::quat(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f)));
        }

        return model;
    }

    void ModelManager::ProcessNode(aiNode* node, const aiScene* scene, const std::string& path)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) 
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            Mesh m = ProcessMesh(mesh, scene, path);
            if (m.GetIndexCount() > 0 || m.GetVertexCount() > 0)
                m_Meshes.push_back(std::move(m));
        }
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
            ProcessNode(node->mChildren[i], scene, path);
    }

    Mesh ModelManager::ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& path)
    {
        std::string meshName = mesh->mName.C_Str();
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // build vertices (unchanged)
        vertices.reserve(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) 
        {
            Vertex v{};
            v.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            v.Normal   = mesh->HasNormals() ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0);
            if (mesh->mTextureCoords[0]) 
            {
                v.TexCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
                if (mesh->HasTangentsAndBitangents()) 
                {
                    v.Tangent   = { mesh->mTangents[i].x,   mesh->mTangents[i].y,   mesh->mTangents[i].z   };
                    v.Bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
                } 
                else 
                {
                    v.Tangent = v.Bitangent = glm::vec3(0);
                }
            } 
            else 
            {
                v.TexCoords = glm::vec2(0);
                v.Tangent = v.Bitangent = glm::vec3(0);
            }
            vertices.push_back(v);
        }

        // indices (unchanged)
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) 
        {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        // --- Materials → textures & engine material -----------------------------
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // sRGB only for DIFFUSE
        auto diffuse  = LoadMaterialTexture(material, aiTextureType_DIFFUSE, path);
        auto normals  = LoadMaterialTexture(material, aiTextureType_NORMALS, path);
        auto specular = LoadMaterialTexture(material, aiTextureType_SPECULAR, path);

        std::vector<std::shared_ptr<Texture>> textures;
        textures.push_back(diffuse);
        textures.push_back(normals);
        textures.push_back(specular);

        Material engineMaterial = toEngineMaterial(material, scene, m_BaseDir, path);

        return Mesh(vertices, indices, engineMaterial, meshName);
    }

    std::shared_ptr<Texture> ModelManager::LoadMaterialTexture(
        aiMaterial* mat,  aiTextureType type, const std::string& path)
    {
        std::shared_ptr<Texture> out;
        const unsigned int count = mat->GetTextureCount(type);
        for (unsigned int i = 0; i < count; ++i) 
        {
            aiString rel;
            if (mat->GetTexture(type, i, &rel) != AI_SUCCESS)
            {
                LOG_WARN("Failed to get texture: {}", rel.C_Str());
                continue;
            } 

            std::filesystem::path newPath = path;
            std::string texName = newPath.stem().string() + "_";

#if defined(_WIN32)
            auto finalPath = newPath.parent_path().string() + "\\textures\\";
#elif defined(__linux__) || defined(__APPLE__)
            auto finalPath = newPath.parent_path().string() + "/textures/";
#endif
            

            // TextureType mapping 
            TextureType ttype = TextureType::DIFFUSE;
            switch (type) 
            {
                case aiTextureType_DIFFUSE:      
                    ttype = TextureType::DIFFUSE;  
                    texName += "diffuse";
                    break;
                case aiTextureType_SPECULAR:     
                    ttype = TextureType::SPECULAR; 
                    texName += "specular"; 
                    break;
                case aiTextureType_NORMALS:     
                    ttype = TextureType::NORMAL;    
                    texName += "normal"; 
                    break;
                default:                          
                    ttype = TextureType::DIFFUSE;  
                    break;
            }

            std::filesystem::path ext = std::filesystem::path(rel.C_Str()).extension();
            
            auto tex = TextureManager::LoadTexture(finalPath + texName + ext.string(), ttype);
            if (tex) 
                out = tex;
        }
        if(count == 0) 
        {                
            std::filesystem::path newPath = path;
            std::string texName = newPath.stem().string() + "_";

#if defined(_WIN32)
            auto finalPath = newPath.parent_path().string() + "\\textures\\";
#elif defined(__linux__) || defined(__APPLE__)
            auto finalPath = newPath.parent_path().string() + "/textures/";
#endif
        
            // TextureType mapping 
            TextureType ttype = TextureType::DIFFUSE;
            switch (type) 
            {
                case aiTextureType_DIFFUSE:      
                    ttype = TextureType::DIFFUSE;  
                    texName += "diffuse";
                    break;
                case aiTextureType_SPECULAR:     
                    ttype = TextureType::SPECULAR; 
                    texName += "specular"; 
                    break;
                case aiTextureType_NORMALS:     
                    ttype = TextureType::NORMAL;    
                    texName += "normal"; 
                    break;
                default:                          
                    ttype = TextureType::DIFFUSE;  
                    break;
            }

            std::string ext = ".jpg";

            
            std::shared_ptr<Texture> tex = TextureManager::LoadTexture(finalPath + texName + ext, ttype);
            if(!tex)
            {
                LOG_INFO("Failed to load texture: {}, trying with .png", finalPath + texName + ext);
                ext = ".png";
                tex = TextureManager::LoadTexture(finalPath + texName + ext, ttype);
                if (!tex)
                {
                    LOG_ERROR("No textures found for: {}", texName);
                    return makeSolidTexture(255, 0, 0, ttype); // return red texture if none found
                }
            }

            out = tex;
        }

        return out;
    }
}
