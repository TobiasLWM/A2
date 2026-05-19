#include <keys.h>
#include <set>

// Keyboard Input
std::set<SDL_Scancode> keys_pressed;
std::set<SDL_Scancode> keys_released;
std::set<SDL_Scancode> keys_down;

// Key pressed handler
SDL_AppResult handle_key_pressed(SDL_Scancode key_code) {
    // Add to key pressed/down list
    keys_pressed.insert(key_code);
    keys_down.insert(key_code);
    return SDL_APP_CONTINUE;
}

// Key released handler
SDL_AppResult handle_key_released(SDL_Scancode key_code) {
    // Add to key released list, remove from key down list
    keys_released.insert(key_code);
    keys_down.erase(key_code);
    return SDL_APP_CONTINUE;
}

// Returns true if key was pressed this frame
bool keyPressedThisFrame(Key key) {
    return keys_pressed.find((SDL_Scancode)key) != keys_pressed.end();
}

// Returns true if key was released this frame
bool keyReleasedThisFrame(Key key) {
    return keys_released.find((SDL_Scancode)key) != keys_released.end();
}

// Returns true if key is currently pressed
bool keyIsPressed(Key key) {
    return keys_down.find((SDL_Scancode)key) != keys_down.end();
}

// Clear keys
void clearKeys() {
    keys_pressed.clear();
    keys_released.clear();
}