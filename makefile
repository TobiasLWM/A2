CC=g++
INCLUDE=-I. -I./Engine/include/ -I./Engine/SDL3/include -I./Engine/SDL3_gfx/include/ -I./Engine/SDL3_gfx/include/SDL3_gfx/  -I./Engine/SDL3_mixer/include/ -I./Engine/SDL3_image/include/
BUILD=build
CFLAGS = -O3
SRC=./

OBJ := $(patsubst ${SRC}/%.cpp,${BUILD}/%.o,$(wildcard ${SRC}/*.cpp))
ENGINE_OBJ := $(patsubst ./Engine/src/%.cpp,./Engine/build/%.o,$(wildcard ./Engine/src/*.cpp))
SDL3_GFX_OBJ := $(patsubst ./Engine/SDL3_gfx/src/%.c,./Engine/SDL3_gfx/build/%.o,$(wildcard ./Engine/SDL3_gfx/src/*.c))

target = platformer

ifeq ($(OS),Windows_NT)
	# Windows
	LIBS = -L. -lSDL3 -lSDL3_image -lSDL3_mixer
	RM = del
	TARGET = ${target}.exe
	CLEAN = del .\build\*.o .\Engine\build\*.o .\Engine\SDL3_gfx\build\ ${TARGET}
else
	# macOS and linux
	LIBS = -L./lib/ -Wl,-rpath,./lib/ -lSDL3 -lSDL3_image -lSDL3_mixer 
	RM = rm
	TARGET = ${target}
	CLEAN = rm ./build/*.o ./Engine/build/*.o ./Engine/SDL3_gfx/build/*.o ${TARGET}
endif

ifeq ($(WASM),1)
    CC=emcc
    LIBS = -L../wasm-libs/ -lSDL3 -lSDL3_image -lSDL3_mixer -sALLOW_MEMORY_GROWTH
    TARGET = ${target}.html
endif

$(TARGET): $(OBJ) $(ENGINE_OBJ) $(SDL3_GFX_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LIBS)

${BUILD}/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $< 

./Engine/build/%.o: ./Engine/src/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $< 

./Engine/SDL3_gfx/build/%.o: ./Engine/SDL3_gfx/src/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $< 

clean:
	${CLEAN}