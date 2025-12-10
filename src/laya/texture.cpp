#include <laya/textures/texture.hpp>
#include <laya/renderers/renderer.hpp>
#include <laya/errors.hpp>

#include <SDL3/SDL.h>
#include <utility>
#include <string>

using namespace std::string_view_literals;

namespace laya {

// ============================================================================
// texture_lock_guard implementation
// ============================================================================

texture_lock_guard::texture_lock_guard(class texture& tex) : m_texture(tex) {
    if (!SDL_LockTexture(tex.native_handle(), nullptr, &m_pixels, &m_pitch)) {
        throw error::from_sdl();
    }
}

texture_lock_guard::~texture_lock_guard() noexcept {
    SDL_UnlockTexture(m_texture.native_handle());
}

void* texture_lock_guard::pixels() const noexcept {
    return m_pixels;
}

int texture_lock_guard::pitch() const noexcept {
    return m_pitch;
}

// ============================================================================
// texture implementation
// ============================================================================

texture::texture(const class renderer& renderer, const texture_args& args) {
    m_texture = SDL_CreateTexture(renderer.native_handle(), static_cast<SDL_PixelFormat>(args.format),
                                  static_cast<SDL_TextureAccess>(args.access), args.size.width, args.size.height);

    if (!m_texture) {
        throw error::from_sdl();
    }
}

texture::texture(const class renderer& renderer, pixel_format format, dimentions size, texture_access access) {
    m_texture = SDL_CreateTexture(renderer.native_handle(), static_cast<SDL_PixelFormat>(format),
                                  static_cast<SDL_TextureAccess>(access), size.width, size.height);

    if (!m_texture) {
        throw error::from_sdl();
    }
}

texture texture::from_surface(const class renderer& renderer, const surface& surf) {
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer.native_handle(), surf.native_handle());
    if (!tex) {
        throw error::from_sdl();
    }
    return texture(tex);
}

texture texture::load_bmp(const class renderer& renderer, std::string_view path) {
    // Load surface first, then convert to texture
    auto surf = surface::load_bmp(path);
    return from_surface(renderer, surf);
}

texture texture::load_png(const class renderer& renderer, std::string_view path) {
    // Load surface first, then convert to texture
    auto surf = surface::load_png(path);  // This will throw since PNG isn't implemented yet
    return from_surface(renderer, surf);
}

texture::~texture() noexcept {
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
    }
}

texture::texture(texture&& other) noexcept : m_texture{std::exchange(other.m_texture, nullptr)} {
}

texture& texture::operator=(texture&& other) noexcept {
    if (this != &other) {
        if (m_texture) {
            SDL_DestroyTexture(m_texture);
        }
        m_texture = std::exchange(other.m_texture, nullptr);
    }
    return *this;
}

void texture::update(const void* pixels, int pitch) {
    if (!SDL_UpdateTexture(m_texture, nullptr, pixels, pitch)) {
        throw error::from_sdl();
    }
}

void texture::update(const rect& r, const void* pixels, int pitch) {
    const SDL_Rect sdl_rect{r.x, r.y, r.w, r.h};
    if (!SDL_UpdateTexture(m_texture, &sdl_rect, pixels, pitch)) {
        throw error::from_sdl();
    }
}

texture_lock_guard texture::lock() {
    return texture_lock_guard(*this);
}

texture_lock_guard texture::lock(const rect& r) {
    // For region locking, we need to use SDL_LockTexture with a rect
    // This is handled by a specialized lock guard constructor
    // For now, we'll implement basic lock and document the limitation
    (void)r;  // Mark parameter as used
    throw error("Regional texture locking not yet implemented: {}", "feature limitation");
}

dimentions texture::size() const noexcept {
    // In SDL3, we use SDL_GetTextureProperties to get texture information
    SDL_PropertiesID props = SDL_GetTextureProperties(m_texture);
    if (props) {
        int w = SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0);
        int h = SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0);
        return {w, h};
    }
    return {0, 0};
}

pixel_format texture::format() const {
    // In SDL3, we use SDL_GetTextureProperties to get texture information
    SDL_PropertiesID props = SDL_GetTextureProperties(m_texture);
    if (props) {
        SDL_PixelFormat format = static_cast<SDL_PixelFormat>(
            SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_FORMAT_NUMBER, SDL_PIXELFORMAT_RGBA32));
        return static_cast<pixel_format>(format);
    }
    return pixel_format::rgba32;  // Default fallback
}

texture_access texture::access() const {
    // In SDL3, we use SDL_GetTextureProperties to get texture information
    SDL_PropertiesID props = SDL_GetTextureProperties(m_texture);
    if (props) {
        SDL_TextureAccess access = static_cast<SDL_TextureAccess>(
            SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_ACCESS_NUMBER, SDL_TEXTUREACCESS_STATIC));
        return static_cast<texture_access>(access);
    }
    return texture_access::static_;  // Default fallback
}

SDL_Texture* texture::native_handle() const noexcept {
    return m_texture;
}

texture::texture(SDL_Texture* tex) : m_texture(tex) {
    // Private constructor for factory methods - takes ownership
}

}  // namespace laya
