# <div align="center"> Isaac's Graphics Engine </div>
<!-- License -->
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-green)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

<div align="center">
    <img src="https://github.com/MatanIsaac/IsaacObjectViewer/blob/dev/iov1.png" alt="Isaac-3D-Object-Viewer">
</div>

---                                                                                                                                    

## Table of Contents

- [General Overview](#general-overview)
- [Goals](#goals)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Getting Started](#getting-started)
  - [Cloning the Repository](#cloning-the-repository)
  - [Building the Project](#building-the-project)
  - [Running the Application](#running-the-application)
- [Usage Overview](#usage-overview)
  - [Mouse Mode — Gizmo Shortcuts](#mouse-mode--gizmo-shortcuts)
  - [Free Camera Mode — Movement Keys](#free-camera-mode--movement-keys)
  - [Panels Overview](#panels-overview)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
                                                                          
---                                                                                                                                                                                                           
## <div align="center"> General Overview </div>

This project is a simple graphics engine created as a side project during my pursuit of a Computer Science degree.                            
While its still a work-in-progress, its purpose is to experiment with and learn 3D graphics programming and rendering techniques.                               
Built with modern C++ and OpenGL, it offers a simple framework for loading, viewing 3D models, and 2D Textures.



## <div align="center"> Goals </div>

The primary goal of this project is to create a simple tool to load, view, and debug 3D objects and 2D textures.    
It aims to serve as:

    - A starting point for understanding OpenGL-based 3D rendering.
    - A development environment for experimenting with shaders, lighting, and rendering techniques.
    - A debugging tool for inspecting 3D object structures and textures.


## <div align="center"> Features </div>

- Rendering
  - Real-time rendering with OpenGL 4.6 Core Profile.
  - Phong/Blinn-Phong specular shading toggle.
  - Directional light + up to 8 point lights with attenuation.
- Assets and Materials
  - Import 3D models via Assimp (OBJ; FBX if your Assimp build includes it).
  - Primitives: Cube, Sphere, Plane.
  - Materials with Diffuse/Specular textures and Shininess control.
  - “Use Material” or “Use Object Color” per object (scene-wide toggle also available).
- Camera and Controls
  - Two modes: Mouse Mode (object selection + gizmo) and Free Camera (W/A/S/D + Q/E).
  - Mouse wheel zoom adjusts camera FOV.
  - ImGuizmo gizmos: Translate (T), Rotate (R), Scale (S), Hide (H).
- UI and Panels (ImGui)
  - Viewport, Scene Hierarchy, Object Settings (Inspector), Scene Settings, Engine Controls, Import dialog.
  - Fixed layout (tabs are not rearrangeable).
  - Environment controls: background color, shading model, camera settings, performance (FPS/VSync).
- Debugging and Tools
  - Line tracer for visualizing mouse/world rays.
  - AABB computed per mesh.
  - Logging with spdlog.
  - Integrated unit tests with Google Test.
- Build & Platform
  - Single Makefile build (app + tests) on Windows and Linux.
  - SDL3 windowing/input and OpenGL context management.

Limitations (current WIP)
- No shadows or post-processing effects yet.
- No skeletal animation/skin weights, PBR, or HDR pipeline.
- UI layout is fixed (no drag-reorder of panels).

## <div align="center"> Prerequisites </div>

Before getting started, make sure you have:

    C++ Compiler: Must support C++17 or later.
    OpenGL: Your graphics drivers should support OpenGL 4.6 or higher 
## <div align="center"> Getting Started </div>

### Cloning the Repository

    git clone https://github.com/MatanIsaac/isaacGraphicsEngine.git
    cd isaacGraphicsEngine

### Building the Project
To Build The Program
    
    make -j 8
To Clean The Program
    
    make clean
To Build The Tests run 
    
    make tests
To Clean The Tests run 
    
    make clean_tests


### Running the Application

Once built, you can start the application with:

    Windows: Run run.bat
    Linux:   Run run.sh

Alternatively, execute the binaries directly from the build directory:

    Application: iov.exe
    Tests: tests/test_runner.exe

## <div align="center"> Usage Overview </div>

The 3D Viewer runs in two distinct modes

| Mode               | Enter Shortcut | Exit Shortcut | What It’s For                                          |
|--------------------|----------------|---------------|--------------------------------------------------------|
| **Mouse Mode**     | *(default)*    | `F4`          | Object selection, gizmo manipulation                   |
| **Free Camera Mode** | `F4`          | `Esc`         | First-person navigation through the scene (WASDQE)     |

---

### Mouse Mode — Gizmo Shortcuts

| Key     | Action            | Description                                  |
|---------|-------------------|----------------------------------------------|
| `H`     | Hide Gizmo        | Toggle gizmo visibility on/off               |
| `T`     | Translate         | Move the selected object along axes          |
| `R`     | Rotate            | Rotate the selected object around axes       |
| `S`     | Scale             | Scale the selected object (per-axis)         |
| `Delete`| Delete Selected   | Remove the selected object from the scene    |

---

### Free Camera Mode — Movement Keys

| Keys                  | Action                 | Description                                            |
|-----------------------|------------------------|--------------------------------------------------------|
| `W` / `S`             |  Move Forward / Back   | Move along the camera view direction                   |
| `A` / `D`             | Strafe Left / Right    | Move sideways (left/right)                             |
| `E`                   | Up                     | Move up                                                |
| `Q`                   | Down                   | Move down                                              |
| `Mouse Scroll Wheel`  | Zoom                   | Adjust camera FOV (narrow = zoom in, widen = zoom out) |

---

### Panels Overview

Note: The UI uses a fixed tab layout; tabs cannot be rearranged.

- Viewport
  - Renders the scene and the active gizmo for the selected object.
  - Click to select in Mouse Mode. T/R/S to switch gizmo, H to hide gizmo. 
  - Press F4 to toggle between Free Camera and Mouse Mode.
  - Shows transform changes in real time when using the gizmo.

- Engine Controls (Top Bar)
  - File: Import 3D Object (.obj/.fbx), Exit.
  - Add: Create primitives (Cube, Sphere, Plane) and Point Lights.
  - Mode: Toggle Mouse Mode / Free Camera (F4).
  - Gizmo Mode: Translate / Rotate / Scale.

- Scene Hierarchy
  - Lists all scene objects (primitives, models, lights).
  - Click to select an object; selection drives the Object Settings panel.

- Object Settings (Inspector)
  - Transform: Position, Rotation, Scale.
  - Material (for primitives and imported models):
    - Use Material / Use Object Color toggle.
    - Object Color picker (shown when Use Material is off).
    - Shininess slider (specular power for Phong/Blinn-Phong).
    - Load Diffuse / Load Specular textures.
  - Light (when a Point Light is selected):
    - Color.
    - Ambient / Diffuse / Specular intensities.
    - Attenuation parameters (if exposed by the UI).

- Scene Settings
  - Background Color.
  - Shading: Phong vs Blinn-Phong toggle (affects specular model).
  - Camera: Position, movement speed, FOV (zoom).
  - Performance: FPS display, VSync toggle, optional FPS cap.

- Import Dialog
  - Opens a file browser to load models/textures.
  - Supported: .obj, .fbx (models); common image formats for textures.

---


## <div align="center"> Project Structure </div>  


    Isaac-Object-Viewer/
    ├── dependencies/           # External libraries and dependencies
    │   ├── assimp/
    │   ├── glad/
    │   ├── glm/
    │   ├── googletest/
    │   ├── imgui/
    │   ├── ImGuizmo/
    │   ├── SDL3/
    │   ├── spdlog/
    │   ├── stb_image.h
    ├── src/                    # Source files
    │   ├── main.cpp            # Main entry point of the application
    │   ├── Engine/             # Engine components
    │   │   ├── Core/           # Core engine systems
    │   │   ├── Graphics/       # Graphics-related components
    │   |   ├── UI/             # UI Managment
    │   ├── Resources/          # Assets and shaders
    │   │   ├── Models/
    │   │   ├── Shaders/
    │   │   └── Textures/
    ├── └── Utility/            # Utility functions and tools


## <div align="center"> Contributing </div>
Thanks for your interest in helping with Isaac's Graphics Engine! I'm just a second-year CS student working on this project as a hobby, so every bit of help is greatly appreciated.

### How to Contribute

- **Report Issues:** Found a bug or have an idea? Please open an issue on our [GitHub Issue Tracker](https://github.com/MatanIsaac/IsaacGraphicsEngine/issues).
- **Improve Documentation:** Simple fixes or clarifications to the docs are very welcome.
- **Submit a Pull Request:** Fork the repo, create a branch off `main`, and open a PR with your changes. Keep things small and clear.

Every contribution, big or small, makes a difference. Thanks for your support!
