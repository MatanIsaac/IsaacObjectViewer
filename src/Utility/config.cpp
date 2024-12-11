#include "config.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string GetProjectRoot()
{
    // Assuming the executable is run from "build" or similar directory.
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path rootPath = currentPath.parent_path().parent_path(); // Adjust based on your project structure
    return rootPath.string();
}