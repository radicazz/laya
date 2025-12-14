/// @file keyboard.cpp
/// @date 2025-12-14

#include <laya/input/keyboard.hpp>
#include <laya/errors.hpp>

#include <SDL3/SDL.h>

namespace laya {

const std::uint8_t* get_keyboard_state(int* num_keys) {
    const auto* state = SDL_GetKeyboardState(num_keys);
    return reinterpret_cast<const std::uint8_t*>(state);
}

bool is_key_pressed(scancode key) {
    int num_keys = 0;
    const auto* state = SDL_GetKeyboardState(&num_keys);
    const auto scan = static_cast<SDL_Scancode>(key);

    if (static_cast<int>(scan) < num_keys) {
        return state[scan];
    }
    return false;
}

bool is_key_pressed(keycode key) {
    const auto scan = SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(key), nullptr);
    return is_key_pressed(static_cast<scancode>(scan));
}

key_modifier get_key_modifiers() {
    return static_cast<key_modifier>(SDL_GetModState());
}

bool has_modifier(key_modifier mod) {
    const auto current = get_key_modifiers();
    const auto current_value = static_cast<std::uint16_t>(current);
    const auto mod_value = static_cast<std::uint16_t>(mod);
    return (current_value & mod_value) == mod_value;
}

keycode scancode_to_keycode(scancode scan) {
    return static_cast<keycode>(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(scan), SDL_KMOD_NONE, false));
}

scancode keycode_to_scancode(keycode key) {
    return static_cast<scancode>(SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(key), nullptr));
}

std::string_view get_scancode_name(scancode scan) {
    const char* name = SDL_GetScancodeName(static_cast<SDL_Scancode>(scan));
    return name ? name : "";
}

std::string_view get_keycode_name(keycode key) {
    const char* name = SDL_GetKeyName(static_cast<SDL_Keycode>(key));
    return name ? name : "";
}

void start_text_input() {
    SDL_StartTextInput(nullptr);
}

void stop_text_input() {
    SDL_StopTextInput(nullptr);
}

bool is_text_input_active() {
    return SDL_TextInputActive(nullptr);
}

void set_text_input_rect(const rect& r) {
    SDL_Rect sdl_rect{r.x, r.y, r.w, r.h};
    SDL_SetTextInputArea(nullptr, &sdl_rect, 0);
}

}  // namespace laya
