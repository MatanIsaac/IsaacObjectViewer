#include "config.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <algorithm>

std::string GetProjectRoot()
{
    // Assuming the executable is run from "build" or similar directory.
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path rootPath = currentPath.parent_path().parent_path(); // Adjust based on your project structure
    return rootPath.string();
}

void ConvertSeparators(std::string& relative_path)
{
    // Construct full path first
    std::string new_path = GetProjectRoot() + OS_SEPARATOR + relative_path;

    // Modify path separators AFTER concatenation
    #ifdef _WIN32
    std::for_each(new_path.begin(), new_path.end(),
        [](char& c) { 
            if (c == '/') 
                c = '\\'; 
        });
    #else
    std::for_each(new_path.begin(), new_path.end(),
        [](char& c) { 
            if (c == '\\') c = '/'; 
        });
    #endif  

    // Assign modified path
    relative_path = new_path;
}