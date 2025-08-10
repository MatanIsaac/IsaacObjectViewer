#pragma once
#include "Utility/config.h"
#include "Model.h"

namespace isaacObjectViewer
{
    class ModelManager 
    {
    public:
        
        static Model* LoadObjModel(const std::string& path);
    
    private:
        ModelManager() { }
    };
}