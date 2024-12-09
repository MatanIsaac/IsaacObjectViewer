# dependencies/glad/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)

# Add GLAD library
add_library(glad STATIC src/glad.c)

# Specify include directories for GLAD
target_include_directories(glad PUBLIC include)

# Optionally, set properties for GLAD
set_target_properties(glad PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)
