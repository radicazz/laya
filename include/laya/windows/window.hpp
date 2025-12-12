#pragma once

#include <optional>
#include <string_view>

#include "window_flags.hpp"
#include "window_id.hpp"

struct SDL_Window;

namespace laya {

/// Arguments for window creation
struct window_args {
    std::string_view title;
    dimensions size{800, 600};
    std::optional<position> initial_position;  ///< Optional initial position
    window_flags flags = window_flags::none;
};

/// RAII wrapper for SDL_Window
class window {
public:
    /// Create a window with the specified arguments
    explicit window(const window_args& args);

    /// Create a window with title, size, and optional flags
    window(std::string_view title, dimensions size, window_flags flags = window_flags::none);

    /// Destructor - automatically destroys the SDL window
    ~window() noexcept;

    // Non-copyable, movable
    window(const window&) = delete;
    window& operator=(const window&) = delete;
    window(window&& other) noexcept;
    window& operator=(window&& other) noexcept;

    /// Show the window
    void show();

    /// Hide the window
    void hide();

    /// Set the window title
    void set_title(std::string_view title);

    /// Set the window size
    void set_size(dimensions size);

    /// Get the window size
    dimensions get_size() const;

    /// Set the window position
    void set_position(position pos);

    /// Get the window position
    position get_position() const;

    /// Maximize the window
    void maximize();

    /// Minimize the window
    void minimize();

    /// Restore the window
    void restore();

    /// Raise the window above other windows
    void raise();

    /// Toggle fullscreen mode
    void set_fullscreen(bool enabled);

    /// Toggle window decorations
    void set_borderless(bool borderless);

    /// Toggle window resizability
    void set_resizable(bool resizable);

    /// Set minimum allowed window size
    void set_minimum_size(dimensions size);

    /// Set maximum allowed window size
    void set_maximum_size(dimensions size);

    /// Get minimum allowed window size
    [[nodiscard]] dimensions get_minimum_size() const;

    /// Get maximum allowed window size
    [[nodiscard]] dimensions get_maximum_size() const;

    /// Set window opacity (0.0 transparent, 1.0 opaque)
    void set_opacity(float opacity);

    /// Get window opacity (0.0 transparent, 1.0 opaque)
    [[nodiscard]] float get_opacity() const;

    /// Enable or disable mouse grab
    void set_mouse_grab(bool grab);

    /// Enable or disable keyboard grab
    void set_keyboard_grab(bool grab);

    /// Get the window ID for event correlation
    [[nodiscard]] window_id id() const noexcept;

    /// Get the native SDL window handle
    [[nodiscard]] SDL_Window* native_handle() const noexcept;

    /// Check if the window owns a valid native handle
    [[nodiscard]] bool is_valid() const noexcept;

private:
    [[nodiscard]] SDL_Window* ensure_handle() const;

    SDL_Window* m_window;
    window_id m_id;  ///< Cache this because it never changes.
};

// ============================================================================
// window inline implementations
// ============================================================================

inline window_id window::id() const noexcept {
    return m_id;
}

inline SDL_Window* window::native_handle() const noexcept {
    return m_window;
}

}  // namespace laya
