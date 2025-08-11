#pragma once
#include "Utility/config.h"
#include "Model.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <unordered_map>

namespace isaacObjectViewer
{
    class ModelManager 
    {
    public:
        static ModelManager& GetInstance()
        {
            static ModelManager instance;
            return instance;
        }
        
        Model* LoadModel(const std::string& path);
        void   ProcessNode(aiNode *node, const aiScene *scene);
        Mesh   ProcessMesh(aiMesh *mesh, const aiScene *scene);

        // now returns shared_ptr<Textures>
        std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(
            aiMaterial *mat, aiTextureType type, const char* debugTypeName);

    private:
        ModelManager() 
            : m_BaseDir()
            , m_Meshes()
        { }

        // robust base-dir (works on / and \)
        std::filesystem::path m_BaseDir;
        std::vector<Mesh>     m_Meshes;

        // canonical-path texture cache to avoid reloading duplicates
        std::unordered_map<std::string, std::weak_ptr<Texture>> m_TextureCache;

        // helpers
        std::string canonicalUnderBase(const std::filesystem::path& p) const;
    };
}
