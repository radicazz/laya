/// @file event_types.hpp
/// @date 2025-10-02
/// @todo - Remove repetition of `state` by extracting it outside and calling it 'input_state'
///       - Add strong types for `timestamp`
///       - variant for window_event::type that accounts for 'data1' and 'data2' meaning

#pragma once

#include <variant>
#include <cstdint>

#include "../windows/window_id.hpp"

// Forward declarations for SDL types
struct SDL_KeyboardEvent;
struct SDL_MouseButtonEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseWheelEvent;
struct SDL_WindowEvent;
struct SDL_QuitEvent;
union SDL_Event;

namespace laya {

/// Application quit event
struct quit_event {
    std::uint32_t timestamp;
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

    std::uint32_t timestamp;
    window_id id;
    type event_type;
    std::int32_t data1;
    std::int32_t data2;
};

/// Keyboard events
struct key_event {
    enum class state { pressed, released };

    std::uint32_t timestamp;
    window_id id;
    state key_state;
    std::uint32_t scancode;  ///< SDL scancode
    std::uint32_t keycode;   ///< SDL keycode
    std::uint16_t mod;       ///< Current key modifiers
    bool repeat;             ///< Non-zero if this is a key repeat
};

/// Text input events
struct text_input_event {
    std::uint32_t timestamp;
    window_id id;
    char text[32];  ///< The input text in UTF-8 encoding
};

/// Text editing events (IME)
struct text_editing_event {
    std::uint32_t timestamp;
    window_id id;
    char text[32];        ///< The editing text in UTF-8 encoding
    std::int32_t start;   ///< The start cursor of selected editing text
    std::int32_t length;  ///< The length of selected editing text
};

/// Mouse motion events
struct mouse_motion_event {
    std::uint32_t timestamp;
    window_id id;
    std::uint32_t which;  ///< The mouse instance id
    std::uint32_t state;  ///< The current button state
    std::int32_t x;       ///< X coordinate, relative to window
    std::int32_t y;       ///< Y coordinate, relative to window
    std::int32_t xrel;    ///< The relative motion in the X direction
    std::int32_t yrel;    ///< The relative motion in the Y direction
};

/// Mouse button events
struct mouse_button_event {
    enum class state { pressed, released };
    enum class button : std::uint8_t { left = 1, middle = 2, right = 3, x1 = 4, x2 = 5 };

    std::uint32_t timestamp;
    window_id id;
    std::uint32_t which;  ///< The mouse instance id
    button mouse_button;  ///< The mouse button index
    state button_state;   ///< Pressed or released
    std::uint8_t clicks;  ///< 1 for single-click, 2 for double-click, etc.
    std::int32_t x;       ///< X coordinate, relative to window
    std::int32_t y;       ///< Y coordinate, relative to window
};

/// Mouse wheel events
struct mouse_wheel_event {
    std::uint32_t timestamp;
    window_id id;
    std::uint32_t which;  ///< The mouse instance id
    std::int32_t x;       ///< The amount scrolled horizontally, positive to the right and negative to the left
    std::int32_t y;       ///< The amount scrolled vertically, positive away from the user and negative toward the user
    float precise_x;  ///< The amount scrolled horizontally, positive to the right and negative to the left, with float
                      ///< precision
    float precise_y;  ///< The amount scrolled vertically, positive away from the user and negative toward the user,
                      ///< with float precision
    std::uint32_t
        direction;  ///< Set to one of the SDL_MOUSEWHEEL_* defines. When FLIPPED the values in X and Y will be opposite
};

/// Joystick axis motion events
struct joystick_axis_event {
    std::uint32_t timestamp;
    std::uint32_t which;  ///< The joystick instance id
    std::uint8_t axis;    ///< The joystick axis index
    std::int16_t value;   ///< The axis value (range: -32768 to 32767)
};

/// Joystick button events
struct joystick_button_event {
    enum class state { pressed, released };

    std::uint32_t timestamp;
    std::uint32_t which;  ///< The joystick instance id
    std::uint8_t button;  ///< The joystick button index
    state button_state;   ///< Pressed or released
};

/// Joystick hat position change events
struct joystick_hat_event {
    std::uint32_t timestamp;
    std::uint32_t which;  ///< The joystick instance id
    std::uint8_t hat;     ///< The joystick hat index
    std::uint8_t value;   ///< The hat position value
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
