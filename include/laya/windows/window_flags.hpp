/// @file window_flags.hpp
/// @date 2025-10-01

#pragma once

#include "../bitmask.hpp"

namespace laya {

/// Window creation and state flags
enum class window_flags : unsigned {
    none = 0x00000000u,
    fullscreen = 0x00000001u,          ///< fullscreen window
    opengl = 0x00000002u,              ///< window usable with OpenGL context
    shown = 0x00000004u,               ///< window is visible
    hidden = 0x00000008u,              ///< window is not visible
    borderless = 0x00000010u,          ///< no window decoration
    resizable = 0x00000020u,           ///< window can be resized
    minimized = 0x00000040u,           ///< window is minimized
    maximized = 0x00000080u,           ///< window is maximized
    mouse_grabbed = 0x00000100u,       ///< window has grabbed mouse input
    input_focus = 0x00000200u,         ///< window has input focus
    mouse_focus = 0x00000400u,         ///< window has mouse focus
    fullscreen_desktop = 0x00001001u,  ///< fullscreen window at desktop resolution
    foreign = 0x00000800u,             ///< window not created by SDL
    high_pixel_density = 0x00002000u,  ///< window should be created in high-DPI mode if supported
    mouse_capture = 0x00004000u,       ///< window has mouse captured
    always_on_top = 0x00008000u,       ///< window should always be above others
    skip_taskbar = 0x00010000u,        ///< window should not be added to the taskbar
    utility = 0x00020000u,             ///< window should be treated as a utility window
    tooltip = 0x00040000u,             ///< window should be treated as a tooltip
    popup_menu = 0x00080000u,          ///< window should be treated as a popup menu
    keyboard_grabbed = 0x00100000u,    ///< window has grabbed keyboard input
    vulkan = 0x10000000u,              ///< window usable for Vulkan surface
    metal = 0x20000000u,               ///< window usable for Metal view
    transparent = 0x01000000u          ///< window with transparent buffer
};

/// Strong types for window properties
struct dimensions {
    int width;
    int height;

    constexpr dimensions(int w, int h) : width(w), height(h) {
    }
};

// Backward compatibility: keep the old misspelling as an alias.
using dimentions = dimensions;

struct position {
    int x;
    int y;

    constexpr position(int x_pos, int y_pos) : x(x_pos), y(y_pos) {
    }
};

/// Enable bitmask operations for window_flags
template <>
struct enable_bitmask_operators<window_flags> : std::true_type {};

}  // namespace laya
