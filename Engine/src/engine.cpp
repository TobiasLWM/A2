#include <iostream>

#define SDL_MAIN_USE_CALLBACKS 1  // use the callbacks instead of main()
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "SDL3_gfx/SDL3_gfxPrimitives.h"
#include "engine.h"
#include "keys.h"
#include "mouse.h"
#include "audio.h"

#include <set>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
Uint64 last = 0;

// Update Methods
GameLoopType gameLoop = GAME_LOOP_HYBRID;
int target_fps = 60;

// Default Window Size
int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

// AppInit - runs once at startup.
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    // Meta Data
    SDL_SetAppMetadata("Massey Game Engine", "1.0", "com.example.massey");

    // Initialise Video and Audio
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialise Audio System
    if(!initAudioSystem()) {
        return SDL_APP_FAILURE;
    }

    // Create Window and Renderer
    if (!SDL_CreateWindowAndRenderer("Massey Game", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Initialise Game
    init();
 
    // Get last
    last = SDL_GetTicksNS();

    return SDL_APP_CONTINUE;  // carry on with the program!
}

// This function runs when a new event (mouse input, keypresses, etc) occurs
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        // End the program, reporting success to the OS
        return SDL_APP_SUCCESS;  
    } else if(event->type == SDL_EVENT_KEY_DOWN) {
        // Key pressed event
        return handle_key_pressed(event->key.scancode);
    } else if(event->type == SDL_EVENT_KEY_UP) {
        // Key released event
        return handle_key_released(event->key.scancode);
    } else if(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        // Mouse pressed event
        return handle_mouse_button_pressed(event->button.button);
    } else if(event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        // Mouse released event
        return handle_mouse_button_released(event->button.button);
    }
    return SDL_APP_CONTINUE;
}

// Fixed Time Step update
// Tries to update and render the game at a fixed rate (target fps)
// This is a bit different to the example in the lecture as we need to fit in with SDL
SDL_AppResult fixedTimeStep() {
    // Expected timestep
    const Time expected = Time::second / target_fps;
    
    // Time of last update
    static Time last;

    // Update audio system
    updateAudioSystem();

    // Update mouse position
    updateInputSystem();

    // Get Time since last update (nanoseconds)
    Time elapsed = getTime() - last;

    // If less time has elapsed than expected (minus 1ms)
    if(elapsed < expected - Time::millisecond) {
        // Sleep for extra time
        // This is only as accurate as the OS scheduler so sleep for less time than expected.
        sleep(expected - elapsed - Time::millisecond);
    }

    // Get current time
    Time now = getTime();

    // Busy waiting loop to get exact time
    while(now - last < expected) now = getTime(); // Busy-waiting

    // Update time of last update
    last = now;

    // Update
    update(expected.seconds);

    // Clear keys
    clearKeys();

    // Clear buttons
    clearButtons();

    // Reset Mouse Delta
    resetMouseDelta();

    // Render
    render(0.0f);

    // Present on screen
    SDL_RenderPresent(renderer);

    // Game should continue
    return SDL_APP_CONTINUE;
}

// Variable Time Step update
// Update and render the game as fast as possible
SDL_AppResult variableTimeStep() {
    // Time of last update
    static Time last = getTime();

    // Update audio system
    updateAudioSystem();

    // Update mouse position
    updateInputSystem();

    // Get Start Time (nanoseconds)
    Time start = getTime();

    // Time since last update
    Time elapsed = (start - last);

    // Update
    update(elapsed.seconds);

    // Clear keys
    clearKeys();

    // Clear buttons
    clearButtons();

    // Reset Mouse Delta
    resetMouseDelta();

    // Render
    render(0.0f);

    // Present on screen
    SDL_RenderPresent(renderer);

    // Update time of last update
    last = start;

    // Game should continue
    return SDL_APP_CONTINUE;
}

// Fixed Update and Variable Render update
// Update the game at a fixed rate and render as fast as possible
SDL_AppResult fixedUpdateVariableRender() {
    // Time of last update
    static Time last = getTime();
    static Time lag;

    // Expected timestep (nanosecond ticks)
    const Time expected = Time::second / target_fps;

    // Update audio system
    updateAudioSystem();

    // Update mouse position
    updateInputSystem();

    // Get current time (nanoseconds)
    Time now = getTime();

    // Increase the lag (how far behind current time the game is)
    lag += now - last;
    last = now;

    // If the lag is greater than expected time
    while(lag > expected) {
        // Update
        update(expected.seconds);

        // Remove from lag
        lag -= expected;

        // Clear keys
        clearKeys();

        // Clear buttons
        clearButtons();

        // Reset Mouse Delta
        resetMouseDelta();
    }

    // Render (convert lag to seconds)
    render(lag.seconds);

    // Present on screen
    SDL_RenderPresent(renderer);

    // Game should continue
    return SDL_APP_CONTINUE;
}

// Main game loop function
SDL_AppResult SDL_AppIterate(void *appstate) {
    if(gameLoop == GAME_LOOP_FIXED) {
        // Fixed Time Step
        fixedTimeStep();
    } else if(gameLoop == GAME_LOOP_VARIABLE) {
        // Variable Time Step
        variableTimeStep();
    } else if(gameLoop == GAME_LOOP_HYBRID) {
        // Fixed Update Variable Render
        fixedUpdateVariableRender();
    } else {
        // Default - Fixed Update Variable Render
        fixedUpdateVariableRender();
    }

    // Continue program
    return SDL_APP_CONTINUE;
}

// Shutdown
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    // Close Audio System
    closeAudioSystem();
}

void setWindowSize(int width, int height) {
    // Set Window Size
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;

    SDL_SetWindowSize(window, width, height);
}

void setWindowTitle(const char *title) {
    // Set Title
    SDL_SetWindowTitle(window, title);
}

void setGameLoopMethod(GameLoopType type) {
    gameLoop = type;
}

void setTargetFPS(int fps) {
    target_fps = fps;
}