# --------------------- Compiler ---------------------
CXX = g++  

# --------------------- Third-Party Paths ---------------------
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
	$(IMGUI_BACKEND)/imgui_impl_opengl3.cpp


# --------------------- OS-Specific ---------------------
EXE =
TEST_EXE =
COPY_RUNTIME =
LDFLAGS =

ifeq ($(OS),Windows_NT)
  # On Windows make runs recipes through cmd.exe. Force it explicitly so the
  # build behaves the same whether launched from PowerShell, cmd, or a Unix-like
  # shell, and use cmd-native file operations instead of Unix mkdir/cp/rm.
  SHELL := cmd.exe
  .SHELLFLAGS := /c

  EXE       = iov.exe
  TEST_EXE  = test_runner.exe
  LDFLAGS   = -Ldependencies/SDL3/x86_64-w64-mingw32/lib -lmingw32 -lSDL3 \
              -Ldependencies/assimp/lib -lassimp -lgdi32 -lopengl32 -limm32 -g
  COPY_RUNTIME = \
    copy /y "dependencies\SDL3\x86_64-w64-mingw32\bin\SDL3.dll" "$(BUILD_DIR)" >nul && \
    copy /y "dependencies\assimp\bin\libassimp-6.dll" "$(BUILD_DIR)" >nul

  # Portable command helpers ($1 = path, forward slashes accepted)
  mkdir_p = if not exist "$(subst /,\,$(patsubst %/,%,$1))" mkdir "$(subst /,\,$(patsubst %/,%,$1))"
  rm_rf   = if exist "$(subst /,\,$1)" rmdir /s /q "$(subst /,\,$1)"
  rm_f    = del /q /f $(subst /,\,$1) 2>nul
else
  EXE       = iov
  TEST_EXE  = test_runner
  # Load shared libs from the exe directory
  LDFLAGS   = -Wl,-rpath,'$$ORIGIN' \
              -lSDL3 -lassimp -lGL -ldl -lpthread -g
  COPY_RUNTIME = cp dependencies/assimp/lib/libassimp.so.6 $(BUILD_DIR)/

  mkdir_p = mkdir -p $1
  rm_rf   = rm -rf $1
  rm_f    = rm -f $1
endif

# --------------------- Includes & Flags ---------------------
INCLUDE = \
	-Idependencies/glad/include \
	-Idependencies/SDL3/x86_64-w64-mingw32/include \
	-Idependencies/spdlog/include \
	-Idependencies/assimp \
	-Idependencies \
	-Isrc \
	-Isrc/Engine \
	-Isrc/Engine/Core \
	-Isrc/Engine/Graphics \
	-Isrc/Engine/UI \
	-I$(IMGUI_DIR) -I$(IMGUI_BACKEND) \
	-I$(IMGUIZMO_DIR) \
	-I$(ImGuiFileDialog_DIR)

CXXFLAGS = -std=c++20 -O0 -g -Wall -Wextra -DIMGUI_DEFINE_MATH_OPERATORS $(INCLUDE)
GLAD_SRC = dependencies/glad/src/glad.c

ifeq ($(OS),Windows_NT)
  CXXFLAGS += -D_CRT_SECURE_NO_WARNINGS -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
else
  CXXFLAGS += -pthread
endif

# --------------------- Build Layout ---------------------
BUILD_DIR := build
OBJDIR    := $(BUILD_DIR)
BINDIR    := $(BUILD_DIR)

# --------------------- Portable recursive wildcard ---------------------
# Usage: $(call rwildcard,dir/,pattern)
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# --------------------- Source Discovery (no external 'find') -----------
SRC_CPP := $(call rwildcard,src/,*.cpp) $(IMGUI_SRC)
SRC_C   := $(GLAD_SRC)
SOURCES := $(SRC_CPP) $(SRC_C)

# Map sources to build/ objects (preserve subdirectories)
OBJ_CPP := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC_CPP))
OBJ_C   := $(patsubst %.c,$(OBJDIR)/%.o,$(SRC_C))
OBJS    := $(OBJ_CPP) $(OBJ_C)

# --------------------- Phony ---------------------
.PHONY: all clean tests clean_tests run compile_commands

# --------------------- IDE: compile_commands.json ---------------------
# Regenerate the clangd compilation database from a dry-run of this Makefile.
PYTHON ?= python
compile_commands:
	$(PYTHON) tools/gen_compile_commands.py

# --------------------- App Build ---------------------
first: $(BINDIR)/$(EXE)
all: $(BINDIR)/$(EXE) tests

# Ensure build root exists 
$(BUILD_DIR):
	@$(call mkdir_p,$(BUILD_DIR))

$(BINDIR)/$(EXE): $(OBJS) | $(BUILD_DIR)
	@$(call mkdir_p,$(dir $@))
	$(CXX) -o $@ $^ $(LDFLAGS)
	@$(COPY_RUNTIME)

# compile into mirrored build/ path; auto-create subdirs
$(OBJDIR)/%.o: %.cpp
	@$(call mkdir_p,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c
	@$(call mkdir_p,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(call rm_rf,$(BUILD_DIR))

# --------------------- GoogleTest ---------------------
GTEST_DIR     = dependencies/googletest/googletest
GTEST_SRC     = $(GTEST_DIR)/src/gtest-all.cc
GTEST_HEADERS = -I$(GTEST_DIR) -I$(GTEST_DIR)/include

TEST_SRCS     := $(call rwildcard,tests/,*.cpp)
TEST_OBJS     := $(patsubst tests/%.cpp,$(OBJDIR)/tests/%.o,$(TEST_SRCS))
TEST_BIN      := $(BINDIR)/tests/$(TEST_EXE)

TEST_CXXFLAGS = -std=c++20 -g -Wall -Wextra -DIMGUI_DEFINE_MATH_OPERATORS $(GTEST_HEADERS) $(INCLUDE)
TEST_LDFLAGS  = $(LDFLAGS)

TEST_COPY_RUNTIME =
ifeq ($(OS),Windows_NT)
  TEST_COPY_RUNTIME = \
	copy /y "dependencies\SDL3\x86_64-w64-mingw32\bin\SDL3.dll" "$(BUILD_DIR)\tests" >nul && \
	copy /y "dependencies\assimp\bin\libassimp-6.dll" "$(BUILD_DIR)\tests" >nul
else
  TEST_COPY_RUNTIME = cp dependencies/assimp/lib/libassimp.so.6 $(BUILD_DIR)/tests/
endif

# A variable for all main application object files *except* for main.o
MAIN_APP_OBJS_NO_MAIN := $(filter-out $(OBJDIR)/src/main.o,$(OBJS))

tests: $(TEST_BIN)

# $(TEST_BIN): $(TEST_OBJS) $(OBJS) $(GTEST_SRC) | $(BUILD_DIR)
$(TEST_BIN): $(TEST_OBJS) $(MAIN_APP_OBJS_NO_MAIN) $(GTEST_SRC) | $(BUILD_DIR)
	@$(call mkdir_p,$(dir $@))
	$(CXX) $(TEST_CXXFLAGS) $^ -o $@ $(TEST_LDFLAGS)
	@$(TEST_COPY_RUNTIME)

$(OBJDIR)/tests/%.o: tests/%.cpp
	@$(call mkdir_p,$(dir $@))
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

clean_tests:
	-$(call rm_f,$(TEST_OBJS) $(TEST_BIN))

clean_all: clean clean_tests

# --------------------- Help ---------------------
.PHONY: help
help:
	@echo "Usage: make [target] [VAR=VALUE]..."
	@echo ""
	@echo "Targets:"
	@echo "  help         Show this help"
	@echo "  make         Build the application binary (default)"
	@echo "  all          Build the application binary (default) & the tests"
	@echo "  tests        Build the GoogleTest runner"
	@echo "  clean        Remove all build artifacts"
	@echo "  clean_tests  Remove only test artifacts"
	@echo "  clean_all    Remove all artifacts"
	@echo "  compile_commands  Regenerate compile_commands.json for clangd/IDE"
	@echo ""
	@echo "Artifacts:"
	@echo "  App:   $(BINDIR)/$(EXE)"
	@echo "  Tests: $(TEST_BIN)"
	@echo ""
	@echo "Variables (override on command line):"
	@echo "  CXX=$(CXX)"
	@echo "  BUILD_DIR=$(BUILD_DIR)"
	@echo "  CXXFLAGS=$(CXXFLAGS)"
	@echo "  LDFLAGS=$(LDFLAGS)"
	@echo "  INCLUDE=$(INCLUDE)"
	@echo ""
	@echo "Examples:"
	@echo "  make"
	@echo "  make tests"
	@echo "  make CXX=clang++ BUILD_DIR=out -j4"