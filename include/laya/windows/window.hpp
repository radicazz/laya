#pragma once

#include <string_view>

#include "window_flags.hpp"
#include "window_id.hpp"

struct SDL_Window;

namespace laya {

/// Arguments for window creation
struct window_args {
    std::string_view title;
    dimentions size{800, 600};
    window_flags flags = window_flags::none;
};

/// RAII wrapper for SDL_Window
class window {
public:
    /// Create a window with the specified arguments
    explicit window(const window_args& args);

    /// Create a window with title, size, and optional flags
    window(std::string_view title, dimentions size, window_flags flags = window_flags::none);

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
    void set_size(dimentions size);

    /// Get the window size
    dimentions get_size() const;

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

    /// Get the window ID for event correlation
    [[nodiscard]] window_id id() const noexcept;

    /// Get the native SDL window handle
    [[nodiscard]] SDL_Window* native_handle() const noexcept;

private:
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
