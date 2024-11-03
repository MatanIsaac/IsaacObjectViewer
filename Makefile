# Set the compiler and flags
CXX = g++
CXXFLAGS = -Wall -pedantic -Wextra -std=c++17 -O2 -g -MMD -MP

# Include directories
INCLUDE_DIRS = -Idependencies/include -Isrc/third_party -Isrc -Idependencies/include/imgui

# Linker flags for GLFW
LIBS = -Ldependencies/libs -lglfw3 -lopengl32 -lgdi32 -luser32

# Object directory
OBJ_DIR = build

# Define source files and object files
SOURCES = src/Camera/Camera.cpp \
          src/Core/Engine.cpp \
          src/Graphics/Cube.cpp \
          src/Graphics/IndexBuffer.cpp \
          src/Graphics/Renderer.cpp \
          src/Graphics/Shader.cpp \
          src/Graphics/Texture.cpp \
          src/Graphics/VertexArray.cpp \
          src/Graphics/VertexBuffer.cpp \
          src/Graphics/VertexBufferLayout.cpp \
          src/Utility/config.cpp \
          src/main.cpp \
          src/third_party/glad.c \
          dependencies/include/imgui/imgui.cpp \
          dependencies/include/imgui/imgui_demo.cpp \
          dependencies/include/imgui/imgui_draw.cpp \
          dependencies/include/imgui/imgui_tables.cpp \
          dependencies/include/imgui/imgui_widgets.cpp \
          dependencies/include/imgui/backends/imgui_impl_glfw.cpp \
          dependencies/include/imgui/backends/imgui_impl_opengl3.cpp

OBJECTS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(patsubst %.c, $(OBJ_DIR)/%.o, $(SOURCES)))

# Define the target executable
TARGET = isaacObjectLoader

# Default rule
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -o $@ $(OBJECTS) $(LIBS)

# Compile source files to object files
$(OBJ_DIR)/%.o: %.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET).exe

# Include dependency files
-include $(OBJECTS:.o=.d)

# Help target
help:
	@echo "Makefile targets:"
	@echo "  all       - Build the target executable"
	@echo "  clean     - Remove all build files"
	@echo "  help      - Display this help message"
