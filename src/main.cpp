#include "Core/Engine.h"
#include "Utility/Log.hpp"

using namespace isaacObjectLoader;

int main()
{
    Log::Init();

    LOG_INFO("Welcome to Isaac's Object Loader!");

    auto engine = Engine::get();
    if (engine)
    {
        engine->RunEngine();
    }

    return 0;
}

/*
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <iostream>

#include "Utility/Log.hpp"

int main()
{
    Log::Init();

    LOG_INFO("Hello Testing Assimp!");

    auto object = aiImportFile("D:/Assets/Weapons/SMG/SMG.fbx", aiProcessPreset_TargetRealtime_MaxQuality);
    if (object == NULL)
    {
        LOG_ERROR("Failed to load asset! - Error: {}", aiGetErrorString());
        return EXIT_FAILURE;
    }

    LOG_INFO("Number of meshes found in file: {}", object->mNumMeshes);
    LOG_INFO("Number of vertices in first mesh: {}", object->mMeshes[0]->mNumVertices);

    return EXIT_SUCCESS;
}
*/