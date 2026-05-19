#ifndef Engine_mouse_h_
#define Engine_mouse_h_

#include <SDL3/SDL.h>
#include <vec2.h>

// Massey GameEngine translation of SDL mouse buttons
enum MouseButton {
    MOUSE_BUTTON_LEFT = SDL_BUTTON_LEFT,
    MOUSE_BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT = SDL_BUTTON_RIGHT
};

// Mouse button pressed handler
SDL_AppResult handle_mouse_button_pressed(Uint8 button);

// Mouse button released handler
SDL_AppResult handle_mouse_button_released(Uint8 button);

// Returns true if mouse button was pressed this frame
bool mouseButtonPressedThisFrame(MouseButton button);

// Returns true if mouse button was released this frame
bool mouseButtonReleasedThisFrame(MouseButton button);

// Returns true if mouse button is currently pressed
bool mouseButtonIsPressed(MouseButton button);

// Returns current mouse position
Vec2 mousePosition();

// Returns mouse delta (change in position) since last poll
Vec2 mouseDelta();

// Update input system
void updateInputSystem();

// Clear mouse buttons
void clearButtons();

// Reset mouse delta
void resetMouseDelta();

#endif