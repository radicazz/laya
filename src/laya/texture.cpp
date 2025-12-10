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

texture_lock_guard::texture_lock_guard(class texture& tex, const rect* region) : m_texture(&tex) {
    SDL_Rect sdl_rect{};
    SDL_Rect* rect_ptr = nullptr;
    if (region) {
        sdl_rect = SDL_Rect{region->x, region->y, region->w, region->h};
        rect_ptr = &sdl_rect;
    }

    if (!SDL_LockTexture(tex.native_handle(), rect_ptr, &m_pixels, &m_pitch)) {
        throw error::from_sdl();
    }
}

texture_lock_guard::texture_lock_guard(texture_lock_guard&& other) noexcept
    : m_texture{std::exchange(other.m_texture, nullptr)}, m_pixels{other.m_pixels}, m_pitch{other.m_pitch} {
}

texture_lock_guard& texture_lock_guard::operator=(texture_lock_guard&& other) noexcept {
    if (this != &other) {
        if (m_texture) {
            SDL_UnlockTexture(m_texture->native_handle());
        }
        m_texture = std::exchange(other.m_texture, nullptr);
        m_pixels = other.m_pixels;
        m_pitch = other.m_pitch;
    }
    return *this;
}

texture_lock_guard::~texture_lock_guard() noexcept {
    if (m_texture) {
        SDL_UnlockTexture(m_texture->native_handle());
    }
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

texture::texture(const class renderer& renderer, const texture_args& args)
    : texture(renderer, args.format, args.size, args.access) {
}

texture::texture(const class renderer& renderer, pixel_format format, dimentions size, texture_access access)
    : m_size{size}, m_format{format}, m_access{access} {
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

    texture result(tex);
    SDL_PropertiesID props = SDL_GetTextureProperties(tex);
    if (props) {
        result.m_size.width = SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_WIDTH_NUMBER, surf.size().width);
        result.m_size.height = SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_HEIGHT_NUMBER, surf.size().height);
        result.m_format = static_cast<pixel_format>(
            SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_FORMAT_NUMBER, static_cast<Sint64>(surf.format())));
        result.m_access = static_cast<texture_access>(
            SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_ACCESS_NUMBER, SDL_TEXTUREACCESS_STATIC));
    } else {
        result.m_size = surf.size();
        result.m_format = surf.format();
        result.m_access = texture_access::static_;
    }

    return result;
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

texture::texture(texture&& other) noexcept
    : m_texture{std::exchange(other.m_texture, nullptr)},
      m_size{std::exchange(other.m_size, dimentions{0, 0})},
      m_format{std::exchange(other.m_format, pixel_format::rgba32)},
      m_access{std::exchange(other.m_access, texture_access::static_)} {
}

texture& texture::operator=(texture&& other) noexcept {
    if (this != &other) {
        if (m_texture) {
            SDL_DestroyTexture(m_texture);
        }
        m_texture = std::exchange(other.m_texture, nullptr);
        m_size = std::exchange(other.m_size, dimentions{0, 0});
        m_format = std::exchange(other.m_format, pixel_format::rgba32);
        m_access = std::exchange(other.m_access, texture_access::static_);
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
    return texture_lock_guard(*this, nullptr);
}

texture_lock_guard texture::lock(const rect& r) {
    return texture_lock_guard(*this, &r);
}

void texture::set_alpha_mod(std::uint8_t alpha) {
    if (!SDL_SetTextureAlphaMod(m_texture, alpha)) {
        throw error::from_sdl();
    }
}

void texture::set_color_mod(color c) {
    if (!SDL_SetTextureColorMod(m_texture, c.r, c.g, c.b)) {
        throw error::from_sdl();
    }
}

void texture::set_color_mod(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    if (!SDL_SetTextureColorMod(m_texture, r, g, b)) {
        throw error::from_sdl();
    }
}

void texture::set_blend_mode(blend_mode mode) {
    if (!SDL_SetTextureBlendMode(m_texture, static_cast<SDL_BlendMode>(mode))) {
        throw error::from_sdl();
    }
}

void texture::set_scale_mode(scale_mode mode) {
    if (!SDL_SetTextureScaleMode(m_texture, static_cast<SDL_ScaleMode>(mode))) {
        throw error::from_sdl();
    }
}

std::uint8_t texture::get_alpha_mod() const {
    std::uint8_t alpha;
    if (!SDL_GetTextureAlphaMod(m_texture, &alpha)) {
        throw error::from_sdl();
    }
    return alpha;
}

color texture::get_color_mod() const {
    std::uint8_t r, g, b;
    if (!SDL_GetTextureColorMod(m_texture, &r, &g, &b)) {
        throw error::from_sdl();
    }
    return color{r, g, b};
}

blend_mode texture::get_blend_mode() const {
    SDL_BlendMode mode;
    if (!SDL_GetTextureBlendMode(m_texture, &mode)) {
        throw error::from_sdl();
    }
    return static_cast<blend_mode>(mode);
}

scale_mode texture::get_scale_mode() const {
    SDL_ScaleMode mode;
    if (!SDL_GetTextureScaleMode(m_texture, &mode)) {
        throw error::from_sdl();
    }
    return static_cast<scale_mode>(mode);
}

dimentions texture::size() const noexcept {
    return m_size;
}

pixel_format texture::format() const {
    return m_format;
}

texture_access texture::access() const {
    return m_access;
}

SDL_Texture* texture::native_handle() const noexcept {
    return m_texture;
}

texture::texture(SDL_Texture* tex) : m_texture(tex) {
    // Private constructor for factory methods - takes ownership
}

}  // namespace laya
