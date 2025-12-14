/// @file mouse.hpp
/// @brief Mouse state queries, cursor control, and strong types
/// @date 2025-12-14

#pragma once

#include <cstdint>

#include "../bitmask.hpp"
#include "../renderers/renderer_types.hpp"  // for point

namespace laya {

// Forward declaration
class window;

// ============================================================================
// Strong types
// ============================================================================

/// Mouse button identifier
enum class mouse_button : std::uint8_t {
    left = 1,
    middle = 2,
    right = 3,
    x1 = 4,  // Extra button 1
    x2 = 5,  // Extra button 2
};

/// Mouse button state flags
enum class mouse_button_mask : std::uint32_t {
    none = 0,
    left = (1u << 0),
    middle = (1u << 1),
    right = (1u << 2),
    x1 = (1u << 3),
    x2 = (1u << 4),
};

template <>
struct enable_bitmask_operators<mouse_button_mask> : std::true_type {};

/// System cursor types
enum class system_cursor {
    arrow,       // Default arrow
    ibeam,       // I-beam for text selection
    wait,        // Wait/busy
    crosshair,   // Crosshair
    wait_arrow,  // Small wait cursor
    size_nwse,   // Double arrow NW-SE
    size_nesw,   // Double arrow NE-SW
    size_we,     // Double arrow W-E
    size_ns,     // Double arrow N-S
    size_all,    // Four pointed arrow
    no,          // Slashed circle or crossbones
    hand,        // Hand pointer
};

// ============================================================================
// Mouse state queries
// ============================================================================

/// Get current mouse position relative to focused window
/// @return Current mouse position, or {0, 0} if no window has focus
[[nodiscard]] point get_mouse_position();

/// Get current mouse position in global screen coordinates
/// @return Global mouse position
[[nodiscard]] point get_global_mouse_position();

/// Get current mouse position relative to specific window
/// @param win Window to query relative position
/// @return Mouse position relative to window
[[nodiscard]] point get_mouse_position(const window& win);

/// Get current mouse button state
/// @return Bitmask of pressed buttons
[[nodiscard]] mouse_button_mask get_mouse_state();

/// Get current mouse position and button state
/// @param x Output: current X position
/// @param y Output: current Y position
/// @return Bitmask of pressed buttons
[[nodiscard]] mouse_button_mask get_mouse_state(int& x, int& y);

/// Check if specific mouse button is pressed
/// @param button Button to check
/// @return true if button is currently pressed
[[nodiscard]] bool is_mouse_button_pressed(mouse_button button);

/// Get relative mouse motion since last call
/// @param x Output: X motion
/// @param y Output: Y motion
/// @return Bitmask of pressed buttons during motion
[[nodiscard]] mouse_button_mask get_relative_mouse_state(int& x, int& y);

// ============================================================================
// Mouse control
// ============================================================================

/// Warp mouse to position in window
/// @param win Target window
/// @param pos Position in window coordinates
void warp_mouse_in_window(window& win, point pos);

/// Warp mouse to global screen position
/// @param pos Global screen coordinates
void warp_mouse_global(point pos);

/// Enable or disable relative mouse mode
/// @param enabled true to enable relative mode (for FPS games, etc.)
/// @note In relative mode, mouse is hidden and position locked, only motion events generated
void set_relative_mouse_mode(bool enabled);

/// Check if relative mouse mode is active
/// @return true if relative mode enabled
[[nodiscard]] bool is_relative_mouse_mode();

/// Capture mouse input (even outside window)
/// @param enabled true to capture mouse
void capture_mouse(bool enabled);

// ============================================================================
// Cursor control
// ============================================================================

/// Show or hide the mouse cursor
/// @param visible true to show cursor, false to hide
void show_cursor(bool visible);

/// Check if cursor is visible
/// @return true if cursor is shown
[[nodiscard]] bool is_cursor_visible();

/// Set cursor to system-defined type
/// @param cursor System cursor to use
void set_cursor(system_cursor cursor);

/// Reset cursor to default arrow
void reset_cursor();

// ============================================================================
// RAII cursor guard
// ============================================================================

/// RAII guard for temporary cursor changes
class cursor_guard {
public:
    /// Set temporary cursor
    explicit cursor_guard(system_cursor temp_cursor);

    /// Restore previous cursor
    ~cursor_guard() noexcept;

    // Non-copyable, non-movable
    cursor_guard(const cursor_guard&) = delete;
    cursor_guard& operator=(const cursor_guard&) = delete;
    cursor_guard(cursor_guard&&) = delete;
    cursor_guard& operator=(cursor_guard&&) = delete;

private:
    // Store previous cursor (implementation detail)
    void* m_previous_cursor;
};

/// Create RAII guard for temporary cursor change
[[nodiscard]] inline cursor_guard with_cursor(system_cursor cursor) {
    return cursor_guard{cursor};
}

}  // namespace laya
