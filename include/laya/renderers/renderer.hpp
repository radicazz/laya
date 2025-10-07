/// @file renderer.hpp
/// @brief Type-safe wrapper for SDL3 renderer interface
/// @date 2025-10-07

#pragma once

#include "renderer_flags.hpp"
#include "renderer_id.hpp"
#include "renderer_types.hpp"

struct SDL_Renderer;

namespace laya {

// Forward declarations
class window;

// ============================================================================
// Renderer creation arguments
// ============================================================================

/// Arguments for renderer creation
struct renderer_args {
    renderer_flags flags = renderer_flags::accelerated;  ///< Renderer creation flags
    vsync_mode vsync = vsync_mode::enabled;              ///< VSync mode (default: enabled)
};

// ============================================================================
// RAII state guards
// ============================================================================

/// RAII guard for draw color state
class draw_color_guard {
public:
    /// Set new draw color and save current state
    draw_color_guard(class renderer& r, color new_color);

    /// Restore previous draw color
    ~draw_color_guard() noexcept;

    // Non-copyable, non-movable
    draw_color_guard(const draw_color_guard&) = delete;
    draw_color_guard& operator=(const draw_color_guard&) = delete;
    draw_color_guard(draw_color_guard&&) = delete;
    draw_color_guard& operator=(draw_color_guard&&) = delete;

private:
    class renderer& m_renderer;
    color m_old_color;
};

/// RAII guard for blend mode state
class blend_mode_guard {
public:
    /// Set new blend mode and save current state
    blend_mode_guard(class renderer& r, blend_mode new_mode);

    /// Restore previous blend mode
    ~blend_mode_guard() noexcept;

    // Non-copyable, non-movable
    blend_mode_guard(const blend_mode_guard&) = delete;
    blend_mode_guard& operator=(const blend_mode_guard&) = delete;
    blend_mode_guard(blend_mode_guard&&) = delete;
    blend_mode_guard& operator=(blend_mode_guard&&) = delete;

private:
    class renderer& m_renderer;
    blend_mode m_old_mode;
};

/// RAII guard for viewport state
class viewport_guard {
public:
    /// Set new viewport and save current state
    viewport_guard(class renderer& r, const rect& new_viewport);

    /// Restore previous viewport
    ~viewport_guard() noexcept;

    // Non-copyable, non-movable
    viewport_guard(const viewport_guard&) = delete;
    viewport_guard& operator=(const viewport_guard&) = delete;
    viewport_guard(viewport_guard&&) = delete;
    viewport_guard& operator=(viewport_guard&&) = delete;

private:
    class renderer& m_renderer;
    rect m_old_viewport;
};

// ============================================================================
// Main renderer class
// ============================================================================

/// RAII wrapper for SDL_Renderer
class renderer {
public:
    /// Create renderer for window with arguments
    explicit renderer(window& win, const renderer_args& args = {});

    /// Create renderer for window with flags
    explicit renderer(window& win, renderer_flags flags);

    /// Destructor - automatically destroys the SDL renderer
    ~renderer() noexcept;

    // Non-copyable, movable
    renderer(const renderer&) = delete;
    renderer& operator=(const renderer&) = delete;
    renderer(renderer&& other) noexcept;
    renderer& operator=(renderer&& other) noexcept;

    // ========================================================================
    // Frame operations
    // ========================================================================

    /// Clear the current rendering target with the current draw color
    void clear();

    /// Update the screen with any rendering performed since the previous call
    void present();

    // ========================================================================
    // State management
    // ========================================================================

    /// Set the color used for drawing operations
    void set_draw_color(color c);

    /// Set the color used for drawing operations (individual components)
    void set_draw_color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255);

    /// Set the blend mode used for drawing operations
    void set_blend_mode(blend_mode mode);

    /// Set the drawing area for rendering on the current target
    void set_viewport(const rect& viewport);

    /// Reset viewport to the entire target
    void reset_viewport();

    /// Get the current draw color
    [[nodiscard]] color get_draw_color() const;

    /// Get the current blend mode
    [[nodiscard]] blend_mode get_blend_mode() const;

    /// Get the current viewport
    [[nodiscard]] rect get_viewport() const;

    /// Get the output size in pixels
    [[nodiscard]] dimentions get_output_size() const;

    // ========================================================================
    // RAII state guard factories
    // ========================================================================

    /// Create RAII guard that temporarily changes draw color
    [[nodiscard]] draw_color_guard with_color(color c);

    /// Create RAII guard that temporarily changes blend mode
    [[nodiscard]] blend_mode_guard with_blend_mode(blend_mode mode);

    /// Create RAII guard that temporarily changes viewport
    [[nodiscard]] viewport_guard with_viewport(const rect& viewport);

    // ========================================================================
    // Primitive drawing operations
    // ========================================================================

    /// Draw a point
    void draw_point(point p);

    /// Draw multiple points
    void draw_points(const point* points, int count);

    /// Draw a line between two points
    void draw_line(point from, point to);

    /// Draw a series of connected lines
    void draw_lines(const point* points, int count);

    /// Draw the outline of a rectangle
    void draw_rect(const rect& r);

    /// Draw the outlines of multiple rectangles
    void draw_rects(const rect* rects, int count);

    /// Fill a rectangle with the current draw color
    void fill_rect(const rect& r);

    /// Fill multiple rectangles with the current draw color
    void fill_rects(const rect* rects, int count);

    // ========================================================================
    // Accessors
    // ========================================================================

    /// Get the renderer ID for correlation with events
    [[nodiscard]] renderer_id id() const noexcept;

    /// Get the native SDL renderer handle
    [[nodiscard]] SDL_Renderer* native_handle() const noexcept;

private:
    SDL_Renderer* m_renderer;
    renderer_id m_id;
};

// ============================================================================
// Inline implementations
// ============================================================================

inline renderer_id renderer::id() const noexcept {
    return m_id;
}

inline SDL_Renderer* renderer::native_handle() const noexcept {
    return m_renderer;
}

}  // namespace laya
