cmake_minimum_required(VERSION 3.27)

# Set project name
project(isaac_object_loader LANGUAGES C CXX VERSION 1.0 DESCRIPTION "Isaacs-Object-Viewer")

# Set C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable position-independent code for static libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable testing
enable_testing()

# Add subdirectories for dependencies
add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/spdlog)
add_subdirectory(dependencies/googletest)
add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/imgui)

# Add subdirectory for project source
add_subdirectory(src)

# Add subdirectory for tests
add_subdirectory(test)
