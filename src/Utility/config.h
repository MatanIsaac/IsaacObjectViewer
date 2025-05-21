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

// GLM library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Constants
constexpr double PI = 3.14159265358979323846;
constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 780;

#include <glad/glad.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

// ImGui core + backends
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

#ifdef _WIN32
    #define OS_SEPARATOR '\\'
#else
    #define OS_SEPARATOR '/'
#endif


std::string GetProjectRoot();
void ConvertSeparators(std::string& relative_path);
