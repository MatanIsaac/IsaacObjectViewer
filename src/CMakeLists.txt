cmake_minimum_required(VERSION 3.27)

# Define the library target
add_library(isaac_object_loader STATIC)

# Include directories for the library
target_include_directories(isaac_object_loader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Utility
    ${CMAKE_SOURCE_DIR}/dependencies
    ${CMAKE_SOURCE_DIR}/dependencies/imgui
    ${CMAKE_SOURCE_DIR}/dependencies/glfw/include
    ${CMAKE_SOURCE_DIR}/dependencies/spdlog/include
)

# Link dependencies to the library
target_link_libraries(isaac_object_loader PUBLIC
    glfw
    glad
    spdlog
)

# Add subdirectories
add_subdirectory(Engine)
add_subdirectory(Utility)

# Create the main executable target
add_executable(iov_exec main.cpp)

# Link the library to the executable
target_link_libraries(iov_exec PUBLIC
    isaac_object_loader
    imgui
    imgui_impl
)

