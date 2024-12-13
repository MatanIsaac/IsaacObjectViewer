# Collect all source files in Utility
file(GLOB UTILITY_SOURCES *.cpp *.h)

# Add sources to the library target
target_sources(isaac_graphics_engine PRIVATE ${UTILITY_SOURCES})

# Include directories for Utility
target_include_directories(isaac_graphics_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
