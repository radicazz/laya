/// @file renderer_types.hpp
/// @brief Core types for 2D rendering operations
/// @date 2025-10-07

#pragma once

#include <cstdint>

#include "../windows/window_flags.hpp"  // For dimensions

namespace laya {

// ============================================================================
// Geometric types
// ============================================================================

/// 2D point with integer coordinates
struct point {
    int x;  ///< X coordinate
    int y;  ///< Y coordinate

    /// Construct point at origin
    constexpr point() noexcept : x(0), y(0) {
    }

    /// Construct point with coordinates
    constexpr point(int x_pos, int y_pos) noexcept : x(x_pos), y(y_pos) {
    }

    /// Equality comparison
    [[nodiscard]] constexpr bool operator==(const point& other) const noexcept {
        return x == other.x && y == other.y;
    }

    /// Inequality comparison
    [[nodiscard]] constexpr bool operator!=(const point& other) const noexcept {
        return !(*this == other);
    }
};

/// Rectangle with integer coordinates and dimensions
struct rect {
    int x;  ///< X coordinate of top-left corner
    int y;  ///< Y coordinate of top-left corner
    int w;  ///< Width
    int h;  ///< Height

    /// Construct empty rectangle at origin
    constexpr rect() noexcept : x(0), y(0), w(0), h(0) {
    }

    /// Construct rectangle with position and size
    constexpr rect(int x_pos, int y_pos, int width, int height) noexcept : x(x_pos), y(y_pos), w(width), h(height) {
    }

    /// Construct rectangle from point and dimensions
    constexpr rect(point pos, dimensions size) noexcept : x(pos.x), y(pos.y), w(size.width), h(size.height) {
    }

    /// Get top-left corner as point
    [[nodiscard]] constexpr point position() const noexcept {
        return {x, y};
    }

    /// Get size as dimensions
    [[nodiscard]] constexpr dimensions size() const noexcept {
        return {w, h};
    }

    /// Equality comparison
    [[nodiscard]] constexpr bool operator==(const rect& other) const noexcept {
        return x == other.x && y == other.y && w == other.w && h == other.h;
    }
};

// ============================================================================
// Color types
// ============================================================================

/// RGBA color with 8-bit components
struct color {
    std::uint8_t r;  ///< Red component (0-255)
    std::uint8_t g;  ///< Green component (0-255)
    std::uint8_t b;  ///< Blue component (0-255)
    std::uint8_t a;  ///< Alpha component (0-255)

    /// Construct opaque black color
    constexpr color() noexcept : r(0), g(0), b(0), a(255) {
    }

    /// Construct color with RGB components (opaque)
    constexpr color(std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept
        : r(red), g(green), b(blue), a(255) {
    }

    /// Construct color with RGBA components
    constexpr color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha) noexcept
        : r(red), g(green), b(blue), a(alpha) {
    }

    /// Equality comparison
    [[nodiscard]] constexpr bool operator==(const color& other) const noexcept {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    /// Inequality comparison
    [[nodiscard]] constexpr bool operator!=(const color& other) const noexcept {
        return !(*this == other);
    }
};

/// RGBA color with floating-point components
struct color_f {
    float r;  ///< Red component (0.0-1.0)
    float g;  ///< Green component (0.0-1.0)
    float b;  ///< Blue component (0.0-1.0)
    float a;  ///< Alpha component (0.0-1.0)

    /// Construct opaque black color
    constexpr color_f() noexcept : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {
    }

    /// Construct color with RGB components (opaque)
    constexpr color_f(float red, float green, float blue) noexcept : r(red), g(green), b(blue), a(1.0f) {
    }

    /// Construct color with RGBA components
    constexpr color_f(float red, float green, float blue, float alpha) noexcept : r(red), g(green), b(blue), a(alpha) {
    }

    /// Equality comparison
    [[nodiscard]] constexpr bool operator==(const color_f& other) const noexcept {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};

// ============================================================================
// Common color constants
// ============================================================================

namespace colors {

constexpr color black{0, 0, 0};
constexpr color white{255, 255, 255};
constexpr color red{255, 0, 0};
constexpr color green{0, 255, 0};
constexpr color blue{0, 0, 255};
constexpr color yellow{255, 255, 0};
constexpr color magenta{255, 0, 255};
constexpr color cyan{0, 255, 255};
constexpr color transparent{0, 0, 0, 0};

}  // namespace colors

// ============================================================================
// Blend modes
// ============================================================================

/// Blend modes for rendering operations
enum class blend_mode : std::uint32_t {
    none = 0x00000000,   ///< No blending
    blend = 0x00000001,  ///< Alpha blending
    add = 0x00000002,    ///< Additive blending
    mod = 0x00000004,    ///< Color modulation
    mul = 0x00000008     ///< Color multiplication
};

// ============================================================================
// Flip modes
// ============================================================================

/// Flip modes for surface and texture transformations
/// Maps directly to SDL_FlipMode values for type safety
enum class flip_mode : int {
    none = 0,        ///< No flipping (SDL_FLIP_NONE)
    horizontal = 1,  ///< Horizontal flip (SDL_FLIP_HORIZONTAL)
    vertical = 2     ///< Vertical flip (SDL_FLIP_VERTICAL)
};

// ============================================================================
// VSync modes
// ============================================================================

/// VSync mode for renderer
enum class vsync_mode : int {
    disabled = 0,  ///< No vsync
    enabled = 1,   ///< Standard vsync
    adaptive = -1  ///< Adaptive vsync (tear if late)
};

}  // namespace laya
