/// RAII wrapper for SDL3 textures with comprehensive texture operations.
/// \file texture.hpp
/// \date 2025-09-28

#pragma once

#include <laya/surfaces/pixel_format.hpp>
#include <laya/surfaces/surface.hpp>
#include <laya/textures/texture_access.hpp>
#include <laya/renderers/renderer_types.hpp>

#include <SDL3/SDL.h>
#include <cstdint>
#include <string_view>
#include <span>

struct SDL_Texture;

namespace laya {

// Forward declarations
class renderer;

/// Arguments for texture construction.
struct texture_args {
    /// Pixel format for the texture.
    pixel_format format = pixel_format::rgba32;

    /// Texture dimensions.
    dimentions size{0, 0};

    /// Access pattern for the texture.
    texture_access access = texture_access::static_;
};

/// RAII guard for texture pixel access during lock operations.
/// Automatically unlocks the texture on destruction.
class texture_lock_guard {
public:
    /// Locks the texture for pixel access.
    /// \param tex Texture to lock.
    /// \throws laya::error if the texture cannot be locked.
    explicit texture_lock_guard(class texture& tex);

    /// Unlocks the texture automatically.
    ~texture_lock_guard() noexcept;

    // Non-copyable but movable
    texture_lock_guard(const texture_lock_guard&) = delete;
    texture_lock_guard& operator=(const texture_lock_guard&) = delete;
    texture_lock_guard(texture_lock_guard&&) = default;
    texture_lock_guard& operator=(texture_lock_guard&&) = default;

    /// Gets raw pixel data pointer.
    /// \returns Non-owning pointer to pixel data.
    [[nodiscard]] void* pixels() const noexcept;

    /// Gets texture pitch (bytes per row).
    /// \returns Number of bytes per row.
    [[nodiscard]] int pitch() const noexcept;

private:
    class texture& m_texture;
    void* m_pixels{nullptr};
    int m_pitch{0};
};

/// RAII wrapper for SDL3 textures.
/// Move-only semantics ensure proper resource management.
class texture {
public:
    // ========================================================================
    // Construction and destruction
    // ========================================================================

    /// Creates a texture with the specified parameters.
    /// \param renderer Renderer to create the texture for.
    /// \param args Texture creation arguments.
    /// \throws laya::error if texture creation fails.
    texture(const class renderer& renderer, const texture_args& args);

    /// Creates a texture with the specified parameters.
    /// \param renderer Renderer to create the texture for.
    /// \param format Pixel format.
    /// \param size Texture dimensions.
    /// \param access Access pattern.
    /// \throws laya::error if texture creation fails.
    texture(const class renderer& renderer, pixel_format format, dimentions size,
            texture_access access = texture_access::static_);

    /// Creates a texture from a surface.
    /// \param renderer Renderer to create the texture for.
    /// \param surf Surface to create texture from.
    /// \throws laya::error if texture creation fails.
    static texture from_surface(const class renderer& renderer, const surface& surf);

    /// Loads a texture from a BMP file.
    /// \param renderer Renderer to create the texture for.
    /// \param path Path to BMP file.
    /// \returns Loaded texture.
    /// \throws laya::error if loading fails.
    static texture load_bmp(const class renderer& renderer, std::string_view path);

    /// Loads a texture from a PNG file (requires SDL_image).
    /// \param renderer Renderer to create the texture for.
    /// \param path Path to PNG file.
    /// \returns Loaded texture.
    /// \throws laya::error if loading fails.
    static texture load_png(const class renderer& renderer, std::string_view path);

    /// Destroys the texture and releases resources.
    ~texture() noexcept;

    // Non-copyable but movable
    texture(const texture&) = delete;
    texture& operator=(const texture&) = delete;
    texture(texture&& other) noexcept;
    texture& operator=(texture&& other) noexcept;

    // ========================================================================
    // State management
    // ========================================================================

    /// Sets alpha modulation for the texture.
    /// \param alpha Alpha value (0-255).
    /// \throws laya::error on SDL failure.
    void set_alpha_mod(std::uint8_t alpha);

    /// Sets color modulation for the texture.
    /// \param c Color modulation.
    /// \throws laya::error on SDL failure.
    void set_color_mod(color c);

    /// Sets color modulation for the texture.
    /// \param r Red component (0-255).
    /// \param g Green component (0-255).
    /// \param b Blue component (0-255).
    /// \throws laya::error on SDL failure.
    void set_color_mod(std::uint8_t r, std::uint8_t g, std::uint8_t b);

    /// Sets blend mode for the texture.
    /// \param mode Blend mode to use.
    /// \throws laya::error on SDL failure.
    void set_blend_mode(blend_mode mode);

    /// Sets scale mode for the texture.
    /// \param mode Scaling mode to use.
    /// \throws laya::error on SDL failure.
    void set_scale_mode(scale_mode mode);

    /// Gets current alpha modulation.
    /// \returns Alpha value (0-255).
    /// \throws laya::error on SDL failure.
    [[nodiscard]] std::uint8_t get_alpha_mod() const;

    /// Gets current color modulation.
    /// \returns Color modulation values.
    /// \throws laya::error on SDL failure.
    [[nodiscard]] color get_color_mod() const;

    /// Gets current blend mode.
    /// \returns Current blend mode.
    /// \throws laya::error on SDL failure.
    [[nodiscard]] blend_mode get_blend_mode() const;

    /// Gets current scale mode.
    /// \returns Current scale mode.
    /// \throws laya::error on SDL failure.
    [[nodiscard]] scale_mode get_scale_mode() const;

    // ========================================================================
    // Pixel access and updates
    // ========================================================================

    /// Updates texture pixels from raw data.
    /// \param pixels Pixel data to upload.
    /// \param pitch Bytes per row.
    /// \throws laya::error on SDL failure.
    void update(const void* pixels, int pitch);

    /// Updates a rectangular region of texture pixels.
    /// \param r Rectangle to update.
    /// \param pixels Pixel data to upload.
    /// \param pitch Bytes per row.
    /// \throws laya::error on SDL failure.
    void update(const rect& r, const void* pixels, int pitch);

    /// Locks the texture for direct pixel access.
    /// \returns Lock guard that automatically unlocks on destruction.
    /// \throws laya::error if the texture cannot be locked.
    texture_lock_guard lock();

    /// Locks a rectangular region for direct pixel access.
    /// \param r Rectangle to lock.
    /// \returns Lock guard that automatically unlocks on destruction.
    /// \throws laya::error if the texture cannot be locked.
    texture_lock_guard lock(const rect& r);

    // ========================================================================
    // Query methods
    // ========================================================================

    /// Gets texture dimensions.
    /// \returns Width and height of the texture.
    [[nodiscard]] dimentions size() const noexcept;

    /// Gets texture pixel format.
    /// \returns Pixel format of the texture.
    [[nodiscard]] pixel_format format() const;

    /// Gets texture access pattern.
    /// \returns Access pattern of the texture.
    [[nodiscard]] texture_access access() const;

    /// Gets the native SDL texture handle.
    /// \returns SDL_Texture pointer for direct SDL operations.
    [[nodiscard]] SDL_Texture* native_handle() const noexcept;

private:
    SDL_Texture* m_texture{nullptr};

    /// Private constructor for factory methods.
    explicit texture(SDL_Texture* tex);

    friend class texture_lock_guard;
};

}  // namespace laya
