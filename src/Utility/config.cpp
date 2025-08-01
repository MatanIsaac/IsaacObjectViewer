#include "config.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <algorithm>

namespace isaacObjectViewer
{
    std::string GetProjectRootPath(const std::string& relative_path)
    {
        std::filesystem::path currentPath = std::filesystem::current_path();

        // Construct full path first
        std::string new_path = currentPath.string() + OS_SEPARATOR + relative_path;
        #ifdef _WIN32
        std::replace(new_path.begin(), new_path.end(), '/', '\\');
        #else
        std::replace(new_path.begin(), new_path.end(), '\\', '/');
        #endif

        return new_path;
    }
}