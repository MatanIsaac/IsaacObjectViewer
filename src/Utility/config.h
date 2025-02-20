#pragma once

// Standard library includes
#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

// GLAD must be included before GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.14159265358979323846
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 780

// ImGui OpenGL loader definition
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include <filesystem>

#ifdef _WIN32
    #define OS_SEPARATOR '\\'
#else
    #define OS_SEPARATOR '/'
#endif


std::string GetProjectRoot();

void ConvertSeparators(std::string& relative_path);
