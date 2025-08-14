/**
 * @brief Manages the loading and processing of 3D models.
 * This class is responsible for loading model files, processing their data,
 * and providing access to the resulting model objects.
 * It uses the Assimp library for importing various 3D model formats.
 */

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

        /// @brief Gets the instance of the ModelManager.
        /// @return The instance of the ModelManager.
        static ModelManager& GetInstance()
        {
            static ModelManager instance;
            return instance;
        }

        /// @brief Loads a 3D model from a file.
        /// @param path The path to the model file.
        /// @return A pointer to the loaded Model object, or nullptr if loading failed.
        Model* LoadModel(const std::string& path);

        /// @brief Processes a node in the Assimp scene.
        /// @param node The node to process.
        /// @param scene The Assimp scene.
        void   ProcessNode(aiNode *node, const aiScene *scene);
        
        /// @brief Processes a mesh in the Assimp scene.
        /// @param mesh The mesh to process.
        /// @param scene The Assimp scene.
        /// @return The processed Mesh object.
        Mesh   ProcessMesh(aiMesh *mesh, const aiScene *scene);

        /// @brief Loads the material textures for a mesh.
        /// @param mat The material to load textures from.
        /// @param type The type of texture to load.
        /// @param debugTypeName A debug name for the texture type.
        /// @return A vector of shared pointers to the loaded textures.
        std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(
            aiMaterial *mat, aiTextureType type, const char* debugTypeName);

    private:
        /// @brief Constructs a ModelManager object.
        ModelManager() 
            : m_BaseDir()
            , m_Meshes()
        { }

        // robust base-dir (works on / and \)
        std::filesystem::path m_BaseDir;
        std::vector<Mesh>     m_Meshes;

        // canonical-path texture cache to avoid reloading duplicates
        std::unordered_map<std::string, std::weak_ptr<Texture>> m_TextureCache;
    
        std::string canonicalUnderBase(const std::filesystem::path& p) const;
    };
}
