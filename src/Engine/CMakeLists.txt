# Collect all source files in Engine
file(GLOB_RECURSE ENGINE_SOURCES Core/*.cpp Core/*.h Graphics/OpenGL/*.cpp Graphics/OpenGL/*.h Scene/**/*.cpp Scene/**/*.h)

# Add sources to the library target
target_sources(isaac_object_loader PRIVATE ${ENGINE_SOURCES})

# Include directories for Engine
target_include_directories(isaac_object_loader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} src)
