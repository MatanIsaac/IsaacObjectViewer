cmake_minimum_required(VERSION 3.27)

# Set project name
project(isaac_graphics_engine LANGUAGES C CXX VERSION 1.0 DESCRIPTION "Isaacs-Graphics-Engine")

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
add_subdirectory(dependencies/assimp)

# Add subdirectory for project source
add_subdirectory(src)

# Add subdirectory for tests
add_subdirectory(test)


# Get the name of the top-level project directory
get_filename_component(PROJECT_DIR_NAME ${CMAKE_SOURCE_DIR} NAME)

# Ensure the directory name is passed as a string literal
target_compile_definitions(isaac_graphics_engine PUBLIC PROJECT_DIR_NAME=\"${PROJECT_DIR_NAME}\")