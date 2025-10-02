/// @file events.hpp
/// @date 2025-10-01

#pragma once

#include <variant>
#include <cstdint>

typedef struct SDL_KeyboardEvent SDL_KeyboardEvent;
typedef struct SDL_MouseButtonEvent SDL_MouseButtonEvent;
typedef struct SDL_MouseMotionEvent SDL_MouseMotionEvent;
typedef struct SDL_MouseWheelEvent SDL_MouseWheelEvent;
typedef struct SDL_WindowEvent SDL_WindowEvent;
typedef struct SDL_QuitEvent SDL_QuitEvent;
typedef union SDL_Event SDL_Event;

namespace laya {

/// Application quit event
struct quit_event {
    uint32_t timestamp;
};

/// Window events (resize, move, close, etc.)
struct window_event {
    enum class type {
        shown,         ///< Window has been shown
        hidden,        ///< Window has been hidden
        exposed,       ///< Window has been exposed and should be redrawn
        moved,         ///< Window has been moved to data1, data2
        resized,       ///< Window has been resized to data1 x data2
        size_changed,  ///< The window size has changed, either as a result of an API call or through the system or user
                       ///< changing the window size
        minimized,     ///< Window has been minimized
        maximized,     ///< Window has been maximized
        restored,      ///< Window has been restored to normal size and position
        enter,         ///< Window has gained mouse focus
        leave,         ///< Window has lost mouse focus
        focus_gained,  ///< Window has gained keyboard focus
        focus_lost,    ///< Window has lost keyboard focus
        close,         ///< The window manager requests that the window be closed
        take_focus,    ///< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or
                       ///< ignore)
        hit_test,      ///< Window had a hit test that wasn't SDL_HITTEST_NORMAL
        icc_profile_changed,  ///< The ICC profile of the window's display has changed
        display_changed       ///< Window has been moved to display data1
    };

    uint32_t timestamp;
    uint32_t window_id;
    type event_type;
    int32_t data1;
    int32_t data2;
};

/// Keyboard events
struct key_event {
    enum class state { pressed, released };

    uint32_t timestamp;
    uint32_t window_id;
    state key_state;
    uint32_t scancode;  ///< SDL scancode
    uint32_t keycode;   ///< SDL keycode
    uint16_t mod;       ///< Current key modifiers
    bool repeat;        ///< Non-zero if this is a key repeat
};

/// Text input events
struct text_input_event {
    uint32_t timestamp;
    uint32_t window_id;
    char text[32];  ///< The input text in UTF-8 encoding
};

/// Text editing events (IME)
struct text_editing_event {
    uint32_t timestamp;
    uint32_t window_id;
    char text[32];   ///< The editing text in UTF-8 encoding
    int32_t start;   ///< The start cursor of selected editing text
    int32_t length;  ///< The length of selected editing text
};

/// Mouse motion events
struct mouse_motion_event {
    uint32_t timestamp;
    uint32_t window_id;
    uint32_t which;  ///< The mouse instance id
    uint32_t state;  ///< The current button state
    int32_t x;       ///< X coordinate, relative to window
    int32_t y;       ///< Y coordinate, relative to window
    int32_t xrel;    ///< The relative motion in the X direction
    int32_t yrel;    ///< The relative motion in the Y direction
};

/// Mouse button events
struct mouse_button_event {
    enum class state { pressed, released };
    enum class button : uint8_t { left = 1, middle = 2, right = 3, x1 = 4, x2 = 5 };

    uint32_t timestamp;
    uint32_t window_id;
    uint32_t which;       ///< The mouse instance id
    button mouse_button;  ///< The mouse button index
    state button_state;   ///< Pressed or released
    uint8_t clicks;       ///< 1 for single-click, 2 for double-click, etc.
    int32_t x;            ///< X coordinate, relative to window
    int32_t y;            ///< Y coordinate, relative to window
};

/// Mouse wheel events
struct mouse_wheel_event {
    uint32_t timestamp;
    uint32_t window_id;
    uint32_t which;   ///< The mouse instance id
    int32_t x;        ///< The amount scrolled horizontally, positive to the right and negative to the left
    int32_t y;        ///< The amount scrolled vertically, positive away from the user and negative toward the user
    float precise_x;  ///< The amount scrolled horizontally, positive to the right and negative to the left, with float
                      ///< precision
    float precise_y;  ///< The amount scrolled vertically, positive away from the user and negative toward the user,
                      ///< with float precision
    uint32_t
        direction;  ///< Set to one of the SDL_MOUSEWHEEL_* defines. When FLIPPED the values in X and Y will be opposite
};

/// Joystick axis motion events
struct joystick_axis_event {
    uint32_t timestamp;
    uint32_t which;  ///< The joystick instance id
    uint8_t axis;    ///< The joystick axis index
    int16_t value;   ///< The axis value (range: -32768 to 32767)
};

/// Joystick button events
struct joystick_button_event {
    enum class state { pressed, released };

    uint32_t timestamp;
    uint32_t which;      ///< The joystick instance id
    uint8_t button;      ///< The joystick button index
    state button_state;  ///< Pressed or released
};

/// Joystick hat position change events
struct joystick_hat_event {
    uint32_t timestamp;
    uint32_t which;  ///< The joystick instance id
    uint8_t hat;     ///< The joystick hat index
    uint8_t value;   ///< The hat position value
};

/// Main event variant containing all possible event types
using event =
    std::variant<quit_event, window_event, key_event, text_input_event, text_editing_event, mouse_motion_event,
                 mouse_button_event, mouse_wheel_event, joystick_axis_event, joystick_button_event, joystick_hat_event>;

/// Convert SDL_Event to laya event
/// @param sdl_event The SDL event to convert
/// @return The converted laya event
event from_sdl_event(const SDL_Event& sdl_event);

}  // namespace laya
