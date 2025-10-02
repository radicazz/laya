#pragma once

#include "window_flags.hpp"
#include <string_view>

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
    uint32_t id() const;

    SDL_Window* native_handle() const;

private:
    SDL_Window* m_window;
};

inline SDL_Window* window::native_handle() const {
    return m_window;
}

}  // namespace laya
