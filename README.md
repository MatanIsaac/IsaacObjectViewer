# isaac's Graphics Engine

<div align="center">
    <img src="https://github.com/MatanIsaac/IsaacGraphicsEngine/blob/main/60_FPS_3500_CUBES.png" alt="Isaac-Graphics-Engine">
</div>

This project is a simple grahics engine created as a side project during my pursuit of a Computer Science degree.                            
Its purpose is to experiment with and learn 3D graphics programming and rendering techniques.                               
Built with modern C++ and OpenGL, it offers a simple framework for loading, viewing 3D models, and 2D Textures.

## Goals

The primary goal of this project is to create a simple tool to load, view, and debug 3D objects and 2D textures.    
It aims to serve as:

    - A starting point for understanding OpenGL-based 3D rendering.
    - A development environment for experimenting with shaders, lighting, and rendering techniques.
    - A debugging tool for inspecting 3D object structures and textures.

## Features

    - CMake Build System: Easily configure, build, and manage dependencies using CMake.
    - GLFW: A cross-platform library for creating windows, receiving input, and managing OpenGL contexts.
    - spdlog: A fast, header-only C++ logging library for providing high-performance logging.
    - google gtest: Utilizing google test framework, a test directory to test your application.
    - imgui: Lightweight and fast graphical user interface library for creating in-app tools, controls, and settings.

## Prerequisites

To get started with this project, you need to have the following installed:

    C++ Compiler: Compatible with C++17 or later.
    CMake: Version 3.27 or higher is recommended.
    OpenGL: Ensure your graphics drivers supports OpenGL 4.0 or higher (there may be further changes and support for DirectX or Vulkan in the future). 

## Getting Started
### Cloning the Repository

    git clone https://github.com/MatanIsaac/isaacGraphicsEngine.git
    cd isaacGraphicsEngine
    
### Building the Project

    Create a build directory:
    mkdir build
    cd build

### Configure the project using CMake:

    cmake ..

### Build the project:

    cmake --build .

### Running the Application

After building, you can run the application/tests from the build directory:

    Application Exec:
    ./build/src/Debug/iov_exec
    Test Exec:
    ./build/test/Debug/TemplateTest

## Project Structure

    Isaac-Object-Loader/
    ├── CMakeLists.txt          # Top-level CMake configuration
    ├── run.bat                 # Script to run the application
    ├── dependencies/           # External libraries and dependencies
    │   ├── assimp/
    │   ├── glad/
    │   ├── glfw/
    │   ├── spdlog/
    │   ├── googletest/
    │   ├── stb_image.h
    ├── src/                    # Source files
    │   ├── CMakeLists.txt      # src CMake configuration
    │   ├── main.cpp            # Main entry point of the application
    │   ├── Engine/             # Engine components
    │   │   ├── CMakeLists.txt      # Engine CMake configuration
    │   │   ├── Core/           # Core engine systems
    │   │   ├── Graphics/       # Graphics-related components
    │   │   │   ├── OpenGL/
    │   │   │   │   ├── Buffers/
    │   │   │   │   ├── Lighting/
    │   │   │   │   ├── Primitives/
    │   │   │   │   ├── Renderer/
    │   │   │   │   ├── Shaders/
    │   │   └── Scene/          # Scene management
    │   │   │   ├── Camera/
    │   │       └── Models/
    │   ├── Resources/          # Assets and shaders
    │   │   ├── Models/
    │   │   ├── Shaders/
    │   │   └── Textures/
    │   └── Utility/            # Utility functions and tools
    │   │   │   ├── CMakeLists.txt      # Utility CMake configuration
    └── test/                   # Test files
        ├── CMakeLists.txt      # CMake configuration for tests


