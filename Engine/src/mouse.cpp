#include <mouse.h>
#include <set>

// Mouse Input
std::set<Uint8> buttons_pressed;
std::set<Uint8> buttons_released;
std::set<Uint8> buttons_down;

// Current
Vec2 mouse_position;
Vec2 mouse_last;
Vec2 mouse_delta;

// Mouse button pressed handler
SDL_AppResult handle_mouse_button_pressed(Uint8 button) {
    // Add to button (mouse) pressed/down list
    buttons_pressed.insert(button);
    buttons_down.insert(button);
    return SDL_APP_CONTINUE;
}

// Mouse button released handler
SDL_AppResult handle_mouse_button_released(Uint8 button) {
    // Add to button (mouse) released list, remove from button down list
    buttons_released.insert(button);
    buttons_down.erase(button);
    return SDL_APP_CONTINUE;
}

// Returns true if mouse button was pressed this frame
bool mouseButtonPressedThisFrame(MouseButton button) {
    return buttons_pressed.find(button) != buttons_pressed.end();
}
// Returns true if mouse button was released this frame
bool mouseButtonReleasedThisFrame(MouseButton button) {
    return buttons_released.find(button) != buttons_released.end();
}
// Returns true if mouse button is currently pressed
bool mouseButtonIsPressed(MouseButton button) {
    return buttons_down.find(button) != buttons_down.end();
}

// Returns current mouse position
Vec2 mousePosition() {
    return mouse_position;
}

// Returns mouse delta (change in position) since last poll
Vec2 mouseDelta() {
    return mouse_delta;
}

// Update Input System mouse position
void updateInputSystem() {
    // Get current mouse position
    SDL_GetMouseState(&mouse_position.x, &mouse_position.y);

    // Calculate mouse delta
    mouse_delta = mouse_position - mouse_last;
}

// Clear mouse buttons
void clearButtons() {
    buttons_pressed.clear();
    buttons_released.clear();
}

// Reset mouse delta
void resetMouseDelta() {
    mouse_last = mouse_position;
}