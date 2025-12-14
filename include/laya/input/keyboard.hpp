/// @file keyboard.hpp
/// @brief Keyboard state queries and strong types for scancodes/keycodes
/// @date 2025-12-14

#pragma once

#include <cstdint>
#include <string_view>

#include "../renderers/renderer_types.hpp"  // for rect
#include "../bitmask.hpp"

namespace laya {

// ============================================================================
// Strong types for keyboard codes
// ============================================================================

/// Physical key position (hardware scancode)
/// Maps to SDL_Scancode - represents physical key location, independent of layout
enum class scancode : std::uint32_t {
    unknown = 0,

    // Alphabet keys
    a = 4,
    b = 5,
    c = 6,
    d = 7,
    e = 8,
    f = 9,
    g = 10,
    h = 11,
    i = 12,
    j = 13,
    k = 14,
    l = 15,
    m = 16,
    n = 17,
    o = 18,
    p = 19,
    q = 20,
    r = 21,
    s = 22,
    t = 23,
    u = 24,
    v = 25,
    w = 26,
    x = 27,
    y = 28,
    z = 29,

    // Number keys
    num_1 = 30,
    num_2 = 31,
    num_3 = 32,
    num_4 = 33,
    num_5 = 34,
    num_6 = 35,
    num_7 = 36,
    num_8 = 37,
    num_9 = 38,
    num_0 = 39,

    // Control keys
    return_ = 40,
    escape = 41,
    backspace = 42,
    tab = 43,
    space = 44,

    // Punctuation
    minus = 45,
    equals = 46,
    leftbracket = 47,
    rightbracket = 48,
    backslash = 49,
    semicolon = 51,
    apostrophe = 52,
    grave = 53,
    comma = 54,
    period = 55,
    slash = 56,

    caps_lock = 57,

    // Function keys
    f1 = 58,
    f2 = 59,
    f3 = 60,
    f4 = 61,
    f5 = 62,
    f6 = 63,
    f7 = 64,
    f8 = 65,
    f9 = 66,
    f10 = 67,
    f11 = 68,
    f12 = 69,

    // Navigation keys
    printscreen = 70,
    scroll_lock = 71,
    pause = 72,
    insert = 73,
    home = 74,
    page_up = 75,
    delete_ = 76,
    end = 77,
    page_down = 78,

    // Arrow keys
    right = 79,
    left = 80,
    down = 81,
    up = 82,

    // Numpad
    num_lock = 83,
    kp_divide = 84,
    kp_multiply = 85,
    kp_minus = 86,
    kp_plus = 87,
    kp_enter = 88,
    kp_1 = 89,
    kp_2 = 90,
    kp_3 = 91,
    kp_4 = 92,
    kp_5 = 93,
    kp_6 = 94,
    kp_7 = 95,
    kp_8 = 96,
    kp_9 = 97,
    kp_0 = 98,
    kp_period = 99,

    // Modifier keys
    lctrl = 224,
    lshift = 225,
    lalt = 226,
    lgui = 227,  // Windows/Command key
    rctrl = 228,
    rshift = 229,
    ralt = 230,
    rgui = 231,
};

/// Virtual key code (keyboard layout-dependent)
/// Maps to SDL_Keycode - represents the key meaning, dependent on layout
enum class keycode : std::uint32_t {
    unknown = 0,
    return_ = '\r',
    escape = 27,
    backspace = '\b',
    tab = '\t',
    space = ' ',
    exclaim = '!',
    quotedbl = '"',
    hash = '#',
    dollar = '$',
    percent = '%',
    ampersand = '&',
    quote = '\'',
    leftparen = '(',
    rightparen = ')',
    asterisk = '*',
    plus = '+',
    comma = ',',
    minus = '-',
    period = '.',
    slash = '/',
    num_0 = '0',
    num_1 = '1',
    num_2 = '2',
    num_3 = '3',
    num_4 = '4',
    num_5 = '5',
    num_6 = '6',
    num_7 = '7',
    num_8 = '8',
    num_9 = '9',
    colon = ':',
    semicolon = ';',
    less = '<',
    equals = '=',
    greater = '>',
    question = '?',
    at = '@',
    leftbracket = '[',
    backslash = '\\',
    rightbracket = ']',
    caret = '^',
    underscore = '_',
    backquote = '`',
    a = 'a',
    b = 'b',
    c = 'c',
    d = 'd',
    e = 'e',
    f = 'f',
    g = 'g',
    h = 'h',
    i = 'i',
    j = 'j',
    k = 'k',
    l = 'l',
    m = 'm',
    n = 'n',
    o = 'o',
    p = 'p',
    q = 'q',
    r = 'r',
    s = 's',
    t = 't',
    u = 'u',
    v = 'v',
    w = 'w',
    x = 'x',
    y = 'y',
    z = 'z',

    // Special keys (using SDL3 keycode values with SDLK_SCANCODE_MASK = 1<<30)
    caps_lock = 0x40000039,
    f1 = 0x4000003A,
    f2 = 0x4000003B,
    f3 = 0x4000003C,
    f4 = 0x4000003D,
    f5 = 0x4000003E,
    f6 = 0x4000003F,
    f7 = 0x40000040,
    f8 = 0x40000041,
    f9 = 0x40000042,
    f10 = 0x40000043,
    f11 = 0x40000044,
    f12 = 0x40000045,
};

/// Key modifier flags
enum class key_modifier : std::uint16_t {
    none = 0x0000,
    lshift = 0x0001,
    rshift = 0x0002,
    lctrl = 0x0040,
    rctrl = 0x0080,
    lalt = 0x0100,
    ralt = 0x0200,
    lgui = 0x0400,
    rgui = 0x0800,
    num_lock = 0x1000,
    caps_lock = 0x2000,
    scroll_lock = 0x4000,

    // Convenience combined flags
    shift = lshift | rshift,
    ctrl = lctrl | rctrl,
    alt = lalt | ralt,
    gui = lgui | rgui,
};

// Enable bitmask operations on key_modifier
template <>
struct enable_bitmask_operators<key_modifier> : std::true_type {};

// ============================================================================
// Keyboard state queries
// ============================================================================

/// Get current keyboard state for all keys
/// @return Pointer to array of key states (1 = pressed, 0 = not pressed)
/// @note Array is indexed by scancode values and remains valid until next SDL_PumpEvents
/// @note This is a direct SDL wrapper - prefer is_key_pressed for single key checks
[[nodiscard]] const std::uint8_t* get_keyboard_state(int* num_keys = nullptr);

/// Check if a specific scancode is currently pressed
/// @param key Physical key scancode to check
/// @return true if key is currently held down
[[nodiscard]] bool is_key_pressed(scancode key);

/// Check if a specific keycode is currently pressed
/// @param key Virtual keycode to check
/// @return true if key is currently held down
[[nodiscard]] bool is_key_pressed(keycode key);

/// Get current keyboard modifier state
/// @return Bitmask of active modifiers
[[nodiscard]] key_modifier get_key_modifiers();

/// Check if specific modifier is active
/// @param mod Modifier to check (can be combined with |)
/// @return true if all specified modifiers are active
[[nodiscard]] bool has_modifier(key_modifier mod);

/// Convert scancode to keycode based on current keyboard layout
/// @param scan Physical scancode
/// @return Corresponding keycode for current layout
[[nodiscard]] keycode scancode_to_keycode(scancode scan);

/// Convert keycode to scancode
/// @param key Virtual keycode
/// @return Corresponding physical scancode
[[nodiscard]] scancode keycode_to_scancode(keycode key);

/// Get human-readable name for scancode
/// @param scan Scancode to name
/// @return String name (e.g., "A", "Space", "Left Ctrl")
[[nodiscard]] std::string_view get_scancode_name(scancode scan);

/// Get human-readable name for keycode
/// @param key Keycode to name
/// @return String name
[[nodiscard]] std::string_view get_keycode_name(keycode key);

// ============================================================================
// Text input control
// ============================================================================

/// Start accepting text input events (text_input_event, text_editing_event)
void start_text_input();

/// Stop accepting text input events
void stop_text_input();

/// Check if text input is currently active
/// @return true if text input mode is enabled
[[nodiscard]] bool is_text_input_active();

/// Set region where text input cursor should be shown (for IME)
/// @param r Rectangle in window coordinates
void set_text_input_rect(const rect& r);

}  // namespace laya
