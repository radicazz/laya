#include <laya/surfaces/surface.hpp>
#include <laya/errors.hpp>

#include <SDL3/SDL.h>
#include <algorithm>
#include <utility>
#include <vector>

using namespace std::string_view_literals;

namespace laya {

// ============================================================================
// surface_lock_guard implementation
// ============================================================================

surface_lock_guard::surface_lock_guard(class surface& surf) : m_surface(surf) {
    if (!SDL_LockSurface(surf.native_handle())) {
        throw error::from_sdl();
    }
}

surface_lock_guard::~surface_lock_guard() noexcept {
    SDL_UnlockSurface(m_surface.native_handle());
}

void* surface_lock_guard::pixels() const noexcept {
    return m_surface.native_handle()->pixels;
}

int surface_lock_guard::pitch() const noexcept {
    return m_surface.native_handle()->pitch;
}

// ============================================================================
// surface implementation
// ============================================================================

surface::surface(const surface_args& args) {
    m_surface = SDL_CreateSurface(args.size.width, args.size.height, static_cast<SDL_PixelFormat>(args.format));

    if (!m_surface) {
        throw error::from_sdl();
    }
}

surface::surface(dimentions size, pixel_format format) {
    m_surface = SDL_CreateSurface(size.width, size.height, static_cast<SDL_PixelFormat>(format));

    if (!m_surface) {
        throw error::from_sdl();
    }
}

surface surface::load_bmp(std::string_view path) {
    SDL_Surface* surf = SDL_LoadBMP(std::string(path).c_str());
    if (!surf) {
        throw error::from_sdl();
    }
    return surface(surf);
}

surface surface::load_png(std::string_view path) {
    // Note: This requires SDL_image
    // SDL_Surface* surf = IMG_Load(std::string(path).c_str());
    // For now, throw an error indicating PNG support requires SDL_image
    (void)path;  // Mark parameter as used
    throw error("PNG loading requires SDL_image library - not yet implemented: {}", "feature not available");
}

surface::~surface() noexcept {
    if (m_surface) {
        SDL_DestroySurface(m_surface);
    }
}

surface::surface(surface&& other) noexcept : m_surface{std::exchange(other.m_surface, nullptr)} {
}

surface& surface::operator=(surface&& other) noexcept {
    if (this != &other) {
        if (m_surface) {
            SDL_DestroySurface(m_surface);
        }
        m_surface = std::exchange(other.m_surface, nullptr);
    }
    return *this;
}

void surface::fill(color c) {
    const std::uint32_t mapped_color = SDL_MapSurfaceRGBA(m_surface, c.r, c.g, c.b, c.a);

    if (!SDL_FillSurfaceRect(m_surface, nullptr, mapped_color)) {
        throw error::from_sdl();
    }
}

void surface::fill_rect(const rect& r, color c) {
    const SDL_Rect sdl_rect{r.x, r.y, r.w, r.h};
    const std::uint32_t mapped_color = SDL_MapSurfaceRGBA(m_surface, c.r, c.g, c.b, c.a);

    if (!SDL_FillSurfaceRect(m_surface, &sdl_rect, mapped_color)) {
        throw error::from_sdl();
    }
}

void surface::fill_rects(std::span<const rect> rects, color c) {
    const std::uint32_t mapped_color = SDL_MapSurfaceRGBA(m_surface, c.r, c.g, c.b, c.a);

    // Convert laya rects to SDL rects
    std::vector<SDL_Rect> sdl_rects;
    sdl_rects.reserve(rects.size());

    std::transform(rects.begin(), rects.end(), std::back_inserter(sdl_rects),
                   [](const rect& r) { return SDL_Rect{r.x, r.y, r.w, r.h}; });

    if (!SDL_FillSurfaceRects(m_surface, sdl_rects.data(), static_cast<int>(sdl_rects.size()), mapped_color)) {
        throw error::from_sdl();
    }
}

void surface::clear() {
    if (!SDL_ClearSurface(m_surface, 0.0f, 0.0f, 0.0f, 0.0f)) {
        throw error::from_sdl();
    }
}

void surface::blit(const surface& src, const rect& src_rect, const rect& dst_rect) {
    const SDL_Rect sdl_src_rect{src_rect.x, src_rect.y, src_rect.w, src_rect.h};
    const SDL_Rect sdl_dst_rect{dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h};

    if (!SDL_BlitSurface(src.m_surface, &sdl_src_rect, m_surface, &sdl_dst_rect)) {
        throw error::from_sdl();
    }
}

void surface::blit(const surface& src, point dst_pos) {
    SDL_Rect sdl_dst_rect{dst_pos.x, dst_pos.y, 0, 0};

    if (!SDL_BlitSurface(src.m_surface, nullptr, m_surface, &sdl_dst_rect)) {
        throw error::from_sdl();
    }
}

void surface::set_alpha_mod(std::uint8_t alpha) {
    if (!SDL_SetSurfaceAlphaMod(m_surface, alpha)) {
        throw error::from_sdl();
    }
}

void surface::set_color_mod(color c) {
    if (!SDL_SetSurfaceColorMod(m_surface, c.r, c.g, c.b)) {
        throw error::from_sdl();
    }
}

void surface::set_color_mod(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    if (!SDL_SetSurfaceColorMod(m_surface, r, g, b)) {
        throw error::from_sdl();
    }
}

void surface::set_blend_mode(blend_mode mode) {
    if (!SDL_SetSurfaceBlendMode(m_surface, static_cast<SDL_BlendMode>(mode))) {
        throw error::from_sdl();
    }
}

void surface::set_color_key(color key) {
    const std::uint32_t mapped_key = SDL_MapSurfaceRGBA(m_surface, key.r, key.g, key.b, key.a);
    if (!SDL_SetSurfaceColorKey(m_surface, true, mapped_key)) {
        throw error::from_sdl();
    }
}

void surface::clear_color_key() {
    if (!SDL_SetSurfaceColorKey(m_surface, false, 0)) {
        throw error::from_sdl();
    }
}

std::uint8_t surface::get_alpha_mod() const {
    std::uint8_t alpha;
    if (!SDL_GetSurfaceAlphaMod(m_surface, &alpha)) {
        throw error::from_sdl();
    }
    return alpha;
}

color surface::get_color_mod() const {
    std::uint8_t r, g, b;
    if (!SDL_GetSurfaceColorMod(m_surface, &r, &g, &b)) {
        throw error::from_sdl();
    }
    return color{r, g, b};
}

blend_mode surface::get_blend_mode() const {
    SDL_BlendMode mode;
    if (!SDL_GetSurfaceBlendMode(m_surface, &mode)) {
        throw error::from_sdl();
    }
    return static_cast<blend_mode>(mode);
}

bool surface::has_color_key() const {
    return SDL_SurfaceHasColorKey(m_surface);
}

color surface::get_color_key() const {
    std::uint32_t key;
    if (!SDL_GetSurfaceColorKey(m_surface, &key)) {
        throw error::from_sdl();
    }
    // For now, return the raw key value as RGBA bytes
    // This is a simplified implementation - proper color key conversion
    // would require more complex pixel format handling
    return color{
        static_cast<std::uint8_t>((key >> 24) & 0xFF),  // R
        static_cast<std::uint8_t>((key >> 16) & 0xFF),  // G
        static_cast<std::uint8_t>((key >> 8) & 0xFF),   // B
        static_cast<std::uint8_t>(key & 0xFF)           // A
    };
}

dimentions surface::size() const noexcept {
    return {m_surface->w, m_surface->h};
}

pixel_format surface::format() const noexcept {
    return static_cast<pixel_format>(m_surface->format);
}

bool surface::must_lock() const noexcept {
    // In SDL3, surfaces generally don't need locking for most operations
    // This is a compatibility function - return false for now
    return false;
}

surface_lock_guard surface::lock() {
    return surface_lock_guard(*this);
}

void surface::save_bmp(std::string_view path) const {
    if (!SDL_SaveBMP(m_surface, std::string(path).c_str())) {
        throw error::from_sdl();
    }
}

void surface::save_png(std::string_view path) const {
    // Note: PNG saving requires SDL_image
    // IMG_SavePNG(m_surface, std::string(path).c_str())
    (void)path;  // Mark parameter as used
    throw error("PNG saving requires SDL_image library - not yet implemented: {}", "feature not available");
}

SDL_Surface* surface::native_handle() const noexcept {
    return m_surface;
}

surface::surface(SDL_Surface* surf) : m_surface(surf) {
    // Private constructor for factory methods - takes ownership
}

}  // namespace laya
