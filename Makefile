CXX = g++
EXE =
COPY_DLL = 
ifeq ($(OS),Windows_NT)
    EXE = ige.exe
    LDFLAGS = -Ldependencies/SDL3/x86_64-w64-mingw32/lib -lmingw32 -lSDL3 -lgdi32 -lopengl32 -limm32 -g
	COPY_DLL = cp dependencies/SDL3/x86_64-w64-mingw32/bin/SDL3.dll .
else
    EXE = ige
    LDFLAGS = -lSDL3 -lGL -ldl -lpthread -g
	COPY_DLL = cp dependencies/SDL3/i686-w64-mingw32/bin/SDL3.dll .
endif


INCLUDE = \
	-Idependencies/glad/include \
	-Idependencies/SDL3/x86_64-w64-mingw32/include \
	-Idependencies/spdlog/include \
	-Idependencies \
	-Isrc

CXXFLAGS = -std=c++17 -g -Wall -Wextra $(INCLUDE)
GLAD_SRC = dependencies/glad/src/glad.c
SOURCES := $(wildcard src/*.cpp src/**/*.cpp src/**/**/*.cpp src/**/**/**/*.cpp src/**/**/**/**/*.cpp) $(GLAD_SRC)
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

