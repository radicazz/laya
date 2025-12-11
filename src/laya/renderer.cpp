/// @file renderer.cpp
/// @brief Implementation of type-safe SDL3 renderer wrapper
/// @date 2025-10-07

#include <utility>
#include <memory>

#include <laya/laya.hpp>
#include <laya/textures/texture.hpp>
#include <SDL3/SDL.h>

namespace laya {

// ============================================================================
// Helper functions
// ============================================================================

namespace {

/// Convert laya blend_mode to SDL blend mode
SDL_BlendMode to_sdl_blend_mode(blend_mode mode) {
    switch (mode) {
        case blend_mode::none:
            return SDL_BLENDMODE_NONE;
        case blend_mode::blend:
            return SDL_BLENDMODE_BLEND;
        case blend_mode::add:
            return SDL_BLENDMODE_ADD;
        case blend_mode::mod:
            return SDL_BLENDMODE_MOD;
        case blend_mode::mul:
            return SDL_BLENDMODE_MUL;
        default:
            return SDL_BLENDMODE_NONE;
    }
}

/// Convert SDL blend mode to laya blend_mode
blend_mode from_sdl_blend_mode(SDL_BlendMode mode) {
    switch (mode) {
        case SDL_BLENDMODE_NONE:
            return blend_mode::none;
        case SDL_BLENDMODE_BLEND:
            return blend_mode::blend;
        case SDL_BLENDMODE_ADD:
            return blend_mode::add;
        case SDL_BLENDMODE_MOD:
            return blend_mode::mod;
        case SDL_BLENDMODE_MUL:
            return blend_mode::mul;
        default:
            return blend_mode::none;
    }
}

/// Convert laya rect to SDL_Rect
SDL_Rect to_sdl_rect(const rect& r) {
    return {r.x, r.y, r.w, r.h};
}

/// Convert SDL_Rect to laya rect
rect from_sdl_rect(const SDL_Rect& r) {
    return {r.x, r.y, r.w, r.h};
}

/// Convert laya point to SDL_Point
SDL_Point to_sdl_point(const point& p) {
    return {p.x, p.y};
}

}  // anonymous namespace

// ============================================================================
// Renderer implementation
// ============================================================================

renderer::renderer(window& win, const renderer_args& args) : m_renderer{nullptr}, m_id{} {
    const char* driver_name = nullptr;
    if ((args.flags & renderer_flags::software) == renderer_flags::software) {
        driver_name = "software";
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    if (props == 0) {
        throw error::from_sdl();
    }

    if (!SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, win.native_handle())) {
        SDL_DestroyProperties(props);
        throw error::from_sdl();
    }
    if (driver_name && !SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, driver_name)) {
        SDL_DestroyProperties(props);
        throw error::from_sdl();
    }

    const bool request_vsync_flag = (args.flags & renderer_flags::present_vsync) == renderer_flags::present_vsync;
    const auto requested_vsync_value =
        static_cast<Sint64>(request_vsync_flag ? static_cast<int>(vsync_mode::enabled) : static_cast<int>(args.vsync));
    if (!SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, requested_vsync_value)) {
        SDL_DestroyProperties(props);
        throw error::from_sdl();
    }

    m_renderer = SDL_CreateRendererWithProperties(props);
    SDL_DestroyProperties(props);
    if (!m_renderer) {
        throw error("Failed to create renderer: {}", SDL_GetError());
    }

    // Set VSync mode (non-fatal if unsupported)
    if (SDL_SetRenderVSync(m_renderer, static_cast<int>(args.vsync)) == false) {
        std::fprintf(stderr, "Warning: Failed to set VSync: %s\n", SDL_GetError());
    }

    // Cache the renderer ID using pointer value as unique identifier
    m_id = renderer_id{static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(m_renderer) & 0xFFFFFFFF)};
}

renderer::renderer(window& win, renderer_flags flags) : renderer(win, renderer_args{flags}) {
}

renderer::~renderer() noexcept {
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
    }
}

renderer::renderer(renderer&& other) noexcept
    : m_renderer{std::exchange(other.m_renderer, nullptr)}, m_id{std::exchange(other.m_id, renderer_id{})} {
}

renderer& renderer::operator=(renderer&& other) noexcept {
    if (this != &other) {
        if (m_renderer) {
            SDL_DestroyRenderer(m_renderer);
        }
        m_renderer = std::exchange(other.m_renderer, nullptr);
        m_id = std::exchange(other.m_id, renderer_id{});
    }
    return *this;
}

// ============================================================================
// Frame operations
// ============================================================================

void renderer::clear() {
    if (SDL_RenderClear(m_renderer) == false) {
        throw error("Failed to clear renderer: {}", SDL_GetError());
    }
}

void renderer::present() {
    if (SDL_RenderPresent(m_renderer) == false) {
        throw error("Failed to present renderer: {}", SDL_GetError());
    }
}

// ============================================================================
// State management
// ============================================================================

void renderer::set_draw_color(color c) {
    if (SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a) == false) {
        throw error("Failed to set draw color: {}", SDL_GetError());
    }
}

void renderer::set_draw_color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) {
    if (SDL_SetRenderDrawColor(m_renderer, r, g, b, a) == false) {
        throw error("Failed to set draw color: {}", SDL_GetError());
    }
}

void renderer::set_blend_mode(blend_mode mode) {
    if (SDL_SetRenderDrawBlendMode(m_renderer, to_sdl_blend_mode(mode)) == false) {
        throw error("Failed to set blend mode: {}", SDL_GetError());
    }
}

void renderer::set_viewport(const rect& viewport) {
    SDL_Rect sdl_rect = to_sdl_rect(viewport);
    if (SDL_SetRenderViewport(m_renderer, &sdl_rect) == false) {
        throw error("Failed to set viewport: {}", SDL_GetError());
    }
}

void renderer::reset_viewport() {
    if (SDL_SetRenderViewport(m_renderer, nullptr) == false) {
        throw error("Failed to reset viewport: {}", SDL_GetError());
    }
}

color renderer::get_draw_color() const {
    std::uint8_t r, g, b, a;
    if (SDL_GetRenderDrawColor(m_renderer, &r, &g, &b, &a) == false) {
        throw error("Failed to get draw color: {}", SDL_GetError());
    }
    return {r, g, b, a};
}

blend_mode renderer::get_blend_mode() const {
    SDL_BlendMode mode;
    if (SDL_GetRenderDrawBlendMode(m_renderer, &mode) == false) {
        throw error("Failed to get blend mode: {}", SDL_GetError());
    }
    return from_sdl_blend_mode(mode);
}

rect renderer::get_viewport() const {
    SDL_Rect viewport;
    if (SDL_GetRenderViewport(m_renderer, &viewport) == false) {
        throw error("Failed to get viewport: {}", SDL_GetError());
    }
    return from_sdl_rect(viewport);
}

dimentions renderer::get_output_size() const {
    int w, h;
    if (SDL_GetRenderOutputSize(m_renderer, &w, &h) == false) {
        throw error("Failed to get output size: {}", SDL_GetError());
    }
    return {w, h};
}

// ============================================================================
// RAII state guard factories
// ============================================================================

draw_color_guard renderer::with_color(color c) {
    return draw_color_guard{*this, c};
}

blend_mode_guard renderer::with_blend_mode(blend_mode mode) {
    return blend_mode_guard{*this, mode};
}

viewport_guard renderer::with_viewport(const rect& viewport) {
    return viewport_guard{*this, viewport};
}

// ============================================================================
// Primitive drawing operations
// ============================================================================

void renderer::draw_point(point p) {
    if (SDL_RenderPoint(m_renderer, static_cast<float>(p.x), static_cast<float>(p.y)) == false) {
        throw error("Failed to draw point: {}", SDL_GetError());
    }
}

void renderer::draw_points(const point* points, int count) {
    if (count <= 0 || !points) {
        return;
    }

    // Convert to SDL FPoints for SDL3
    auto sdl_points = std::make_unique<SDL_FPoint[]>(count);
    for (int i = 0; i < count; ++i) {
        sdl_points[i] = {static_cast<float>(points[i].x), static_cast<float>(points[i].y)};
    }

    if (SDL_RenderPoints(m_renderer, sdl_points.get(), count) == false) {
        throw error("Failed to draw points: {}", SDL_GetError());
    }
}

void renderer::draw_line(point from, point to) {
    if (SDL_RenderLine(m_renderer, static_cast<float>(from.x), static_cast<float>(from.y), static_cast<float>(to.x),
                       static_cast<float>(to.y)) == false) {
        throw error("Failed to draw line: {}", SDL_GetError());
    }
}

void renderer::draw_lines(const point* points, int count) {
    if (count <= 1 || !points) {
        return;
    }

    // Convert to SDL FPoints for SDL3
    auto sdl_points = std::make_unique<SDL_FPoint[]>(count);
    for (int i = 0; i < count; ++i) {
        sdl_points[i] = {static_cast<float>(points[i].x), static_cast<float>(points[i].y)};
    }

    if (SDL_RenderLines(m_renderer, sdl_points.get(), count) == false) {
        throw error("Failed to draw lines: {}", SDL_GetError());
    }
}

void renderer::draw_rect(const rect& r) {
    SDL_FRect sdl_rect{static_cast<float>(r.x), static_cast<float>(r.y), static_cast<float>(r.w),
                       static_cast<float>(r.h)};
    if (SDL_RenderRect(m_renderer, &sdl_rect) == false) {
        throw error("Failed to draw rect: {}", SDL_GetError());
    }
}

void renderer::draw_rects(const rect* rects, int count) {
    if (count <= 0 || !rects) {
        return;
    }

    // Convert to SDL rects
    auto sdl_rects = std::make_unique<SDL_FRect[]>(count);
    for (int i = 0; i < count; ++i) {
        sdl_rects[i] = {static_cast<float>(rects[i].x), static_cast<float>(rects[i].y), static_cast<float>(rects[i].w),
                        static_cast<float>(rects[i].h)};
    }

    if (SDL_RenderRects(m_renderer, sdl_rects.get(), count) == false) {
        throw error("Failed to draw rects: {}", SDL_GetError());
    }
}

void renderer::fill_rect(const rect& r) {
    SDL_FRect sdl_rect{static_cast<float>(r.x), static_cast<float>(r.y), static_cast<float>(r.w),
                       static_cast<float>(r.h)};
    if (SDL_RenderFillRect(m_renderer, &sdl_rect) == false) {
        throw error("Failed to fill rect: {}", SDL_GetError());
    }
}

void renderer::fill_rects(const rect* rects, int count) {
    if (count <= 0 || !rects) {
        return;
    }

    // Convert to SDL rects
    auto sdl_rects = std::make_unique<SDL_FRect[]>(count);
    for (int i = 0; i < count; ++i) {
        sdl_rects[i] = {static_cast<float>(rects[i].x), static_cast<float>(rects[i].y), static_cast<float>(rects[i].w),
                        static_cast<float>(rects[i].h)};
    }

    if (SDL_RenderFillRects(m_renderer, sdl_rects.get(), count) == false) {
        throw error("Failed to fill rects: {}", SDL_GetError());
    }
}

// ============================================================================
// Texture rendering operations
// ============================================================================

void renderer::render(const texture& tex, point dst_pos) {
    auto tex_size = tex.size();
    SDL_FRect dst_rect{static_cast<float>(dst_pos.x), static_cast<float>(dst_pos.y), static_cast<float>(tex_size.width),
                       static_cast<float>(tex_size.height)};

    if (!SDL_RenderTexture(m_renderer, tex.native_handle(), nullptr, &dst_rect)) {
        throw error("Failed to render texture: {}", SDL_GetError());
    }
}

void renderer::render(const texture& tex, const rect& dst_rect) {
    SDL_FRect sdl_dst{static_cast<float>(dst_rect.x), static_cast<float>(dst_rect.y), static_cast<float>(dst_rect.w),
                      static_cast<float>(dst_rect.h)};

    if (!SDL_RenderTexture(m_renderer, tex.native_handle(), nullptr, &sdl_dst)) {
        throw error("Failed to render texture: {}", SDL_GetError());
    }
}

void renderer::render(const texture& tex, const rect& src_rect, const rect& dst_rect) {
    SDL_FRect sdl_src{static_cast<float>(src_rect.x), static_cast<float>(src_rect.y), static_cast<float>(src_rect.w),
                      static_cast<float>(src_rect.h)};
    SDL_FRect sdl_dst{static_cast<float>(dst_rect.x), static_cast<float>(dst_rect.y), static_cast<float>(dst_rect.w),
                      static_cast<float>(dst_rect.h)};

    if (!SDL_RenderTexture(m_renderer, tex.native_handle(), &sdl_src, &sdl_dst)) {
        throw error("Failed to render texture: {}", SDL_GetError());
    }
}

void renderer::render(const texture& tex, const rect& dst_rect, double angle) {
    SDL_FRect sdl_dst{static_cast<float>(dst_rect.x), static_cast<float>(dst_rect.y), static_cast<float>(dst_rect.w),
                      static_cast<float>(dst_rect.h)};

    if (!SDL_RenderTextureRotated(m_renderer, tex.native_handle(), nullptr, &sdl_dst, angle, nullptr, SDL_FLIP_NONE)) {
        throw error("Failed to render rotated texture: {}", SDL_GetError());
    }
}

void renderer::render(const texture& tex, const rect& dst_rect, double angle, point center) {
    SDL_FRect sdl_dst{static_cast<float>(dst_rect.x), static_cast<float>(dst_rect.y), static_cast<float>(dst_rect.w),
                      static_cast<float>(dst_rect.h)};
    SDL_FPoint sdl_center{static_cast<float>(center.x), static_cast<float>(center.y)};

    if (!SDL_RenderTextureRotated(m_renderer, tex.native_handle(), nullptr, &sdl_dst, angle, &sdl_center,
                                  SDL_FLIP_NONE)) {
        throw error("Failed to render rotated texture: {}", SDL_GetError());
    }
}

void renderer::render(const texture& tex, const rect& src_rect, const rect& dst_rect, double angle, point center,
                      flip_mode flip) {
    SDL_FRect sdl_src{static_cast<float>(src_rect.x), static_cast<float>(src_rect.y), static_cast<float>(src_rect.w),
                      static_cast<float>(src_rect.h)};
    SDL_FRect sdl_dst{static_cast<float>(dst_rect.x), static_cast<float>(dst_rect.y), static_cast<float>(dst_rect.w),
                      static_cast<float>(dst_rect.h)};
    SDL_FPoint sdl_center{static_cast<float>(center.x), static_cast<float>(center.y)};

    if (!SDL_RenderTextureRotated(m_renderer, tex.native_handle(), &sdl_src, &sdl_dst, angle, &sdl_center,
                                  static_cast<SDL_FlipMode>(flip))) {
        throw error("Failed to render rotated texture: {}", SDL_GetError());
    }
}

void renderer::render(const texture& tex, const rect& dst_rect, flip_mode flip) {
    SDL_FRect sdl_dst{static_cast<float>(dst_rect.x), static_cast<float>(dst_rect.y), static_cast<float>(dst_rect.w),
                      static_cast<float>(dst_rect.h)};

    if (!SDL_RenderTextureRotated(m_renderer, tex.native_handle(), nullptr, &sdl_dst, 0.0, nullptr,
                                  static_cast<SDL_FlipMode>(flip))) {
        throw error("Failed to render flipped texture: {}", SDL_GetError());
    }
}

void renderer::render(const texture& tex, const rect& src_rect, const rect& dst_rect, flip_mode flip) {
    SDL_FRect sdl_src{static_cast<float>(src_rect.x), static_cast<float>(src_rect.y), static_cast<float>(src_rect.w),
                      static_cast<float>(src_rect.h)};
    SDL_FRect sdl_dst{static_cast<float>(dst_rect.x), static_cast<float>(dst_rect.y), static_cast<float>(dst_rect.w),
                      static_cast<float>(dst_rect.h)};

    if (!SDL_RenderTextureRotated(m_renderer, tex.native_handle(), &sdl_src, &sdl_dst, 0.0, nullptr,
                                  static_cast<SDL_FlipMode>(flip))) {
        throw error("Failed to render flipped texture: {}", SDL_GetError());
    }
}

// ============================================================================
// RAII state guard implementations
// ============================================================================

draw_color_guard::draw_color_guard(renderer& r, color new_color) : m_renderer{r}, m_old_color{r.get_draw_color()} {
    m_renderer.set_draw_color(new_color);
}

draw_color_guard::~draw_color_guard() noexcept {
    try {
        m_renderer.set_draw_color(m_old_color);
    } catch (...) {
        // Ignore exceptions in destructor
    }
}

blend_mode_guard::blend_mode_guard(renderer& r, blend_mode new_mode) : m_renderer{r}, m_old_mode{r.get_blend_mode()} {
    m_renderer.set_blend_mode(new_mode);
}

blend_mode_guard::~blend_mode_guard() noexcept {
    try {
        m_renderer.set_blend_mode(m_old_mode);
    } catch (...) {
        // Ignore exceptions in destructor
    }
}

viewport_guard::viewport_guard(renderer& r, const rect& new_viewport)
    : m_renderer{r}, m_old_viewport{r.get_viewport()} {
    m_renderer.set_viewport(new_viewport);
}

viewport_guard::~viewport_guard() noexcept {
    try {
        m_renderer.set_viewport(m_old_viewport);
    } catch (...) {
        // Ignore exceptions in destructor
    }
}

}  // namespace laya
