#pragma once

// Standard library includes
#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <memory>

// GLM library
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

// Constants
constexpr double PI = 3.14159265358979323846;
constexpr int SCREEN_WIDTH = 1600;
constexpr int SCREEN_HEIGHT = 900;

#include <glad/glad.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

// ImGui core + backends
#include "imgui.h"
#include <imgui_internal.h>
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"


#ifdef _WIN32
    #define OS_SEPARATOR '\\'
#else
    #define OS_SEPARATOR '/'
#endif

namespace isaacObjectViewer
{
    std::string GetProjectRootPath(const std::string& relative_path);
}