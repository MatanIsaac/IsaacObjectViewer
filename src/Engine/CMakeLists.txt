# Collect all source files in Engine
file(GLOB_RECURSE ENGINE_SOURCES Core/*.cpp Core/*.h Graphics/OpenGL/*.cpp Graphics/OpenGL/*.h Scene/**/*.cpp Scene/**/*.h)

# Add sources to the library target
target_sources(isaac_graphics_engine PRIVATE ${ENGINE_SOURCES})

# Include directories for Engine
target_include_directories(isaac_graphics_engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine
    ${CMAKE_CURRENT_SOURCE_DIR}/Utility
)
