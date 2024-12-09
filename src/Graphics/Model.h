#pragma once
#include "Shader.h"
#include "Mesh.h"
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace isaacObjectLoader
{
    unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

    class Model
    {
    public:
        Model(char *path)
        {
            LoadModel(path);
        }

        void Render(Shader &shader);

    private:
        // model data
        // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<MeshTexture> m_TexturesLoaded;
        std::vector<Mesh> m_Meshes;
        std::string m_Directory;

        void LoadModel(std::string path);
        void ProcessNode(aiNode *node, const aiScene *scene);
        Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<MeshTexture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    };
}