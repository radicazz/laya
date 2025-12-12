/// @file surface.hpp
/// @brief Surface RAII wrapper for SDL3 surface operations
/// @date 2025-12-10

#pragma once

#include <cstdint>
#include <span>
#include <string_view>

#include "../renderers/renderer_types.hpp"
#include "../windows/window_flags.hpp"
#include "pixel_format.hpp"
#include "surface_flags.hpp"

// Forward declarations
struct SDL_Surface;

namespace laya {

/// Arguments for surface creation
struct surface_args {
    dimensions size;                             ///< Surface dimensions
    pixel_format format = pixel_format::rgba32;  ///< Pixel format
    surface_flags flags = surface_flags::none;   ///< Creation flags (currently only rle_optimized is applied)
};

/// RAII lock guard for surface pixel access
class surface_lock_guard {
public:
    explicit surface_lock_guard(class surface& surf);
    ~surface_lock_guard() noexcept;

    // Non-copyable but movable
    surface_lock_guard(const surface_lock_guard&) = delete;
    surface_lock_guard& operator=(const surface_lock_guard&) = delete;
    surface_lock_guard(surface_lock_guard&& other) noexcept;
    surface_lock_guard& operator=(surface_lock_guard&& other) noexcept;

    /// Get direct pixel data pointer
    [[nodiscard]] void* pixels() const noexcept;

    /// Get pitch (row stride in bytes)
    [[nodiscard]] int pitch() const noexcept;

private:
    class surface* m_surface;
    void* m_pixels{nullptr};
    int m_pitch{0};
};

/// RAII wrapper for SDL_Surface
class surface {
public:
    // Creation
    explicit surface(const surface_args& args);
    surface(dimensions size, pixel_format format = pixel_format::rgba32);

    /// Load surface from BMP file
    [[nodiscard]] static surface load_bmp(std::string_view path);

    /// Load surface from PNG file (requires SDL_image)
    [[nodiscard]] static surface load_png(std::string_view path);

    // RAII
    ~surface() noexcept;
    surface(const surface&) = delete;
    surface& operator=(const surface&) = delete;
    surface(surface&& other) noexcept;
    surface& operator=(surface&& other) noexcept;

    // Operations (throw laya::error on failure)
    void fill(color c);
    void fill_rect(const rect& r, color c);
    void fill_rects(std::span<const rect> rects, color c);
    void clear();
    void blit(const surface& src, const rect& src_rect, const rect& dst_rect);
    void blit(const surface& src, point dst_pos);

    // Transformations (return new surface)
    [[nodiscard]] surface convert(pixel_format format) const;
    [[nodiscard]] surface duplicate() const;
    [[nodiscard]] surface scale(dimensions new_size) const;
    [[nodiscard]] surface flip(flip_mode mode) const;

    // State management
    void set_alpha_mod(std::uint8_t alpha);
    void set_color_mod(color c);
    void set_color_mod(std::uint8_t r, std::uint8_t g, std::uint8_t b);
    void set_blend_mode(blend_mode mode);
    void set_color_key(color key);
    void clear_color_key();

    // Getters (noexcept)
    [[nodiscard]] std::uint8_t get_alpha_mod() const;
    [[nodiscard]] color get_color_mod() const;
    [[nodiscard]] blend_mode get_blend_mode() const;
    [[nodiscard]] bool has_color_key() const;
    [[nodiscard]] color get_color_key() const;
    [[nodiscard]] dimensions size() const noexcept;
    [[nodiscard]] pixel_format format() const noexcept;
    [[nodiscard]] bool must_lock() const noexcept;

    // Low-level access
    [[nodiscard]] surface_lock_guard lock();
    void save_bmp(std::string_view path) const;
    void save_png(std::string_view path) const;

    /// Get native SDL surface handle
    [[nodiscard]] SDL_Surface* native_handle() const noexcept;

private:
    SDL_Surface* m_surface;
    explicit surface(SDL_Surface* surf);  // For factory methods
};

}  // namespace laya
