# <div align="center"> Isaac's Graphics Engine </div>
<!-- License -->
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)


<div align="center">
    <img src="https://github.com/MatanIsaac/IsaacObjectViewer/blob/dev/iov1.png" alt="Isaac-3D-Object-Viewer">
</div>

---                                                                                                                                                                                                              

This project is a simple graphics engine created as a side project during my pursuit of a Computer Science degree.                            
While its still a work-in-progress, its purpose is to experiment with and learn 3D graphics programming and rendering techniques.                               
Built with modern C++ and OpenGL, it offers a simple framework for loading, viewing 3D models, and 2D Textures.

## <div align="center"> 🎯 Goals </div>

The primary goal of this project is to create a simple tool to load, view, and debug 3D objects and 2D textures.    
It aims to serve as:

    - A starting point for understanding OpenGL-based 3D rendering.
    - A development environment for experimenting with shaders, lighting, and rendering techniques.
    - A debugging tool for inspecting 3D object structures and textures.


## <div align="center"> ✨ Features </div>

    Single Makefile Build System: Handles configuration, building, and dependency management, as well as handling tests.
    SDL3: Seamless creation of windows, input handling, and OpenGL context management.
    spdlog: Fast, header-only logging for high-performance output.
    Google Test (gtest): Integrated tests to help ensure code quality.
    ImGui (including ImGuizmo): Lightweight, in-app GUI library for creating tools, controls, and settings.

## <div align="center"> 🛠️ Prerequisites </div>

Before getting started, make sure you have:

    C++ Compiler: Must support C++17 or later.
    OpenGL: Your graphics drivers should support OpenGL 4.6 or higher (with potential future support for DirectX or Vulkan).

## <div align="center"> 🚀 Getting Started </div>

### Cloning the Repository

    git clone https://github.com/MatanIsaac/isaacGraphicsEngine.git
    cd isaacGraphicsEngine

### Building the Project
Simply Run the command
    
    Make 
To Build Tests run 
    
    make tests


### Running the Application

Once built, you can start the application with:

    Windows: Run run.bat
    Linux:   Run run.sh

Alternatively, execute the binaries directly from the build directory:

    Application: iov.exe
    Tests: tests/test_runner.exe

## <div align="center"> 🕹️ Usage Overview </div>

The 3D Viewer runs in two distinct modes

| Mode               | Enter Shortcut | Exit Shortcut | What It’s For                                          |
|--------------------|----------------|---------------|--------------------------------------------------------|
| **Mouse Mode**     | *(default)*    | `F4`          | Object selection, gizmo manipulation                   |
| **Free Camera Mode** | `F4`          | `Esc`         | First-person navigation through the scene (WASDQE)     |

---

### Mouse Mode — Gizmo Shortcuts

| Key | Action        |
|-----|---------------|
| `T` | **T**ranslate |
| `R` | **R**otate    |
| `S` | **S**cale     |

---

### Free Camera Mode — Movement Keys

| Direction | Keys |
|-----------|------|
| Forward / Back | `W` / `S` |
| Strafe Left / Right | `A` / `D` |
| Ascend | `E` |
| Descend | `Q` |

---

## <div align="center"> 📁 Project Structure </div>  


    Isaac-Object-Viewer/
    ├── dependencies/           # External libraries and dependencies
    │   ├── glad/
    │   ├── glm/
    │   ├── googletest/
    │   ├── imgui/
    │   ├── ImGuizmo/
    │   ├── SDL3/
    │   ├── spdlog/
    │   ├── stb_image.h
    │   ├── tiny_obj_loader.h
    ├── src/                    # Source files
    │   ├── main.cpp            # Main entry point of the application
    │   ├── Engine/             # Engine components
    │   │   ├── Core/           # Core engine systems
    │   │   ├── Graphics/       # Graphics-related components
    │   │   │   ├── Buffers/
    │   │   │   ├── Lighting/
    │   │   │   ├── Primitives/
    │   │   │   ├── Renderer/
    │   │   │   ├── Shaders/
    │   │   └── Scene/          # Scene management
    │   │   │   ├── Camera/
    │   │       └── Models/
    │   ├── Resources/          # Assets and shaders
    │   │   ├── Models/
    │   │   ├── Shaders/
    │   │   └── Textures/
    ├── └── Utility/            # Utility functions and tools


## <div align="center"> 🤝 Contributing </div>
Thanks for your interest in helping with Isaac's Graphics Engine! I'm just a second-year CS student working on this project as a hobby, so every bit of help is greatly appreciated.

### How to Contribute

- **Report Issues:** Found a bug or have an idea? Please open an issue on our [GitHub Issue Tracker](https://github.com/MatanIsaac/IsaacGraphicsEngine/issues).
- **Improve Documentation:** Simple fixes or clarifications to the docs are very welcome.
- **Submit a Pull Request:** Fork the repo, create a branch off `main`, and open a PR with your changes. Keep things small and clear.

Every contribution, big or small, makes a difference. Thanks for your support!
