/// @file event_window.hpp
/// @date 2025-10-02
/// @brief Type-safe window events with std::variant for data fields

#pragma once

#include <variant>
#include <cstdint>
#include <optional>

#include "../windows/window_id.hpp"

namespace laya {

// ============================================================================
// Window event data types
// ============================================================================

/// No additional data for the event
struct window_event_data_none {};

/// Position data for window move events
struct window_event_data_position {
    std::int32_t x;
    std::int32_t y;
};

/// Size data for window resize/size_changed events
struct window_event_data_size {
    std::int32_t width;
    std::int32_t height;
};

/// Display index for display_changed events
struct window_event_data_display {
    std::int32_t display_index;
};

/// Variant holding all possible window event data types
using window_event_data =
    std::variant<window_event_data_none, window_event_data_position, window_event_data_size, window_event_data_display>;

// ============================================================================
// Window event type enum
// ============================================================================

/// Window event types
enum class window_event_type {
    shown,                ///< Window has been shown
    hidden,               ///< Window has been hidden
    exposed,              ///< Window has been exposed and should be redrawn
    moved,                ///< Window has been moved
    resized,              ///< Window has been resized
    size_changed,         ///< The window size has changed
    minimized,            ///< Window has been minimized
    maximized,            ///< Window has been maximized
    restored,             ///< Window has been restored to normal size and position
    enter,                ///< Window has gained mouse focus
    leave,                ///< Window has lost mouse focus
    focus_gained,         ///< Window has gained keyboard focus
    focus_lost,           ///< Window has lost keyboard focus
    close,                ///< The window manager requests that the window be closed
    take_focus,           ///< Window is being offered a focus
    hit_test,             ///< Window had a hit test that wasn't SDL_HITTEST_NORMAL
    icc_profile_changed,  ///< The ICC profile of the window's display has changed
    display_changed       ///< Window has been moved to a different display
};

// ============================================================================
// Window event structure
// ============================================================================

/// Window events (resize, move, close, etc.)
struct window_event {
    std::uint32_t timestamp;
    window_id id;
    window_event_type event_type;
    window_event_data data;
};

// ============================================================================
// Helper functions for type-safe data access
// ============================================================================

/// Get position data from window event
/// @param event The window event to query
/// @return Position if event type is moved, std::nullopt otherwise
[[nodiscard]] inline std::optional<window_event_data_position> get_position(const window_event& event) noexcept {
    if (event.event_type == window_event_type::moved) {
        if (const auto* pos = std::get_if<window_event_data_position>(&event.data)) {
            return *pos;
        }
    }
    return std::nullopt;
}

/// Get size data from window event
/// @param event The window event to query
/// @return Size if event type is resized or size_changed, std::nullopt otherwise
[[nodiscard]] inline std::optional<window_event_data_size> get_size(const window_event& event) noexcept {
    if (event.event_type == window_event_type::resized || event.event_type == window_event_type::size_changed) {
        if (const auto* size = std::get_if<window_event_data_size>(&event.data)) {
            return *size;
        }
    }
    return std::nullopt;
}

/// Get display index from window event
/// @param event The window event to query
/// @return Display index if event type is display_changed, std::nullopt otherwise
[[nodiscard]] inline std::optional<window_event_data_display> get_display(const window_event& event) noexcept {
    if (event.event_type == window_event_type::display_changed) {
        if (const auto* display = std::get_if<window_event_data_display>(&event.data)) {
            return *display;
        }
    }
    return std::nullopt;
}

}  // namespace laya
