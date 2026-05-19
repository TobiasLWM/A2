#ifndef Engine_h_
#define Engine_h_

#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <audio.h>
#include <keys.h>
#include <mouse.h>
#include <random.h>
#include <render.h>
#include <timing.h>
#include <vec2.h>

// Size of Window
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

// Max Function
template<class T>
T max(T a, T b) {
    return (a > b) ? a : b;
}

// Min Function
template<class T>
T min(T a, T b) {
    return (a < b) ? a : b;
}

// Game Loop Type
enum GameLoopType {
    GAME_LOOP_FIXED,
    GAME_LOOP_VARIABLE,
    GAME_LOOP_HYBRID
};

void setWindowSize(int width, int height);
void setWindowTitle(const char *title);
void setGameLoopMethod(GameLoopType type);
void setTargetFPS(int fps);

void init();
void update(float dt);
void render(float lag);
void close();

#endif
