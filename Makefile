CXX = g++

IMGUI_DIR = dependencies/imgui
IMGUIZMO_DIR = dependencies/ImGuizmo
ImGuiFileDialog_DIR = dependencies/ImGuiFileDialog
IMGUI_BACKEND = $(IMGUI_DIR)/backends
IMGUI_SRC = \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/imgui_demo.cpp \
	$(IMGUIZMO_DIR)/ImGuizmo.cpp \
	$(ImGuiFileDialog_DIR)/ImGuiFileDialog.cpp \
	$(IMGUI_BACKEND)/imgui_impl_sdl3.cpp \
	$(IMGUI_BACKEND)/imgui_impl_opengl3.cpp \

# --------------------- Compiler and Linker Settings ---------------------
EXE =
COPY_DLL = 
LDFLAGS = 
ifeq ($(OS),Windows_NT)
    EXE = iov.exe
	LDFLAGS = -Ldependencies/SDL3/x86_64-w64-mingw32/lib -lmingw32 -lSDL3 -Ldependencies/assimp/lib -lassimp -lgdi32 -lopengl32 -limm32 -g # -Wl,-subsystem,windows
	COPY_DLL = cp dependencies/SDL3/x86_64-w64-mingw32/bin/SDL3.dll dependencies/assimp/bin/libassimp-6.dll .
else
    EXE = iov
    LDFLAGS = -lSDL3 -lassimp -lGL -ldl -lpthread -g
endif


INCLUDE = \
	-Idependencies/glad/include \
	-Idependencies/SDL3/x86_64-w64-mingw32/include \
	-Idependencies/spdlog/include \
	-Idependencies/assimp \
	-Idependencies\assimp\bin \
	-Idependencies\assimp\lib \
	-Idependencies \
	-Isrc \
	-Isrc/Engine \
	-Isrc/Engine/Core \
	-Isrc/Engine/Graphics \
	-Isrc/Engine/UI \
	-I$(IMGUI_DIR) -I$(IMGUI_BACKEND) \
	-I$(IMGUIZMO_DIR) \
	-I$(ImGuiFileDialog_DIR)/ImGuiFileDialog.hpp \

CXXFLAGS = -std=c++20 -g -Wall -DIMGUI_DEFINE_MATH_OPERATORS $(INCLUDE)
GLAD_SRC = dependencies/glad/src/glad.c
SOURCES := $(wildcard src/*.cpp src/**/*.cpp src/**/**/*.cpp src/**/**/**/*.cpp src/**/**/**/**/*.cpp) $(GLAD_SRC) $(IMGUI_SRC)
OBJS := $(SOURCES:.cpp=.o)
OBJS := $(OBJS:.c=.o)

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)
	$(COPY_DLL)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXE)

# --------------------- GoogleTest ---------------------
GTEST_DIR     = dependencies/googletest/googletest
GTEST_SRC     = $(GTEST_DIR)/src/gtest-all.cc
GTEST_HEADERS = -I$(GTEST_DIR) -I$(GTEST_DIR)/include

TEST_SRCS     := $(wildcard tests/*.cpp)
TEST_OBJS     := $(TEST_SRCS:.cpp=.o)
TEST_BIN      := tests/test_runner

TEST_CXXFLAGS = -std=c++17 -g -Wall -Wextra -DIMGUI_DEFINE_MATH_OPERATORS $(GTEST_HEADERS) $(INCLUDE)
TEST_LDFLAGS  = $(LDFLAGS)

tests: $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJS) $(OBJS) $(GTEST_SRC)
	$(CXX) $(TEST_CXXFLAGS) $^ -o $@ $(TEST_LDFLAGS)

tests/%.o: tests/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

clean_tests:
	rm -f $(TEST_OBJS) $(TEST_BIN)
# --------------------- GoogleTest ---------------------
