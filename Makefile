EXE = ige.exe
CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra \
  -Idependencies/glad/include \
  -Idependencies/SDL3/x86_64-w64-mingw32/include \
  -Idependencies/spdlog/include \
  -Idependencies \
  -Isrc

LDFLAGS = -Ldependencies/SDL3/x86_64-w64-mingw32/lib -lmingw32 -lSDL3 -lgdi32 -lopengl32 -limm32 -g

GLAD_SRC = dependencies/glad/src/glad.c

SOURCES := $(wildcard src/*.cpp src/**/*.cpp src/**/**/*.cpp src/**/**/**/*.cpp src/**/**/**/**/*.cpp) $(GLAD_SRC)
OBJS := $(SOURCES:.cpp=.o)
OBJS := $(OBJS:.c=.o)

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)
	cp dependencies/SDL3/x86_64-w64-mingw32/bin/SDL3.dll .

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXE)
