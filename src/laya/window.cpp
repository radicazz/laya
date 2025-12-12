#include <cstdint>
#include <string>
#include <utility>

#include <laya/windows/window.hpp>
#include <laya/errors.hpp>
#include <SDL3/SDL.h>

namespace laya {

namespace {

template <typename Setter, typename... Args>
void set_or_throw(SDL_PropertiesID props, Setter setter, Args&&... args) {
    if (!setter(props, std::forward<Args>(args)...)) {
        SDL_DestroyProperties(props);
        throw error::from_sdl();
    }
}

SDL_PropertiesID build_window_properties(const window_args& args) {
    SDL_PropertiesID props = SDL_CreateProperties();
    if (props == 0) {
        throw error::from_sdl();
    }

    const std::string title{args.title};
    set_or_throw(props, SDL_SetStringProperty, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title.c_str());
    set_or_throw(props, SDL_SetNumberProperty, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER,
                 static_cast<Sint64>(args.size.width));
    set_or_throw(props, SDL_SetNumberProperty, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER,
                 static_cast<Sint64>(args.size.height));
    set_or_throw(props, SDL_SetNumberProperty, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER,
                 static_cast<Sint64>(static_cast<std::uint64_t>(args.flags)));

    if (args.initial_position) {
        set_or_throw(props, SDL_SetNumberProperty, SDL_PROP_WINDOW_CREATE_X_NUMBER,
                     static_cast<Sint64>(args.initial_position->x));
        set_or_throw(props, SDL_SetNumberProperty, SDL_PROP_WINDOW_CREATE_Y_NUMBER,
                     static_cast<Sint64>(args.initial_position->y));
    }

    if ((args.flags & window_flags::high_pixel_density) == window_flags::high_pixel_density) {
        set_or_throw(props, SDL_SetBooleanProperty, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
    }

    return props;
}

}  // namespace

window::window(const window_args& args) : m_window{nullptr}, m_id{} {
    SDL_PropertiesID props = build_window_properties(args);
    m_window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);

    if (!m_window) {
        throw error::from_sdl();
    }

    m_id = window_id{SDL_GetWindowID(m_window)};
    if (!m_id.is_valid()) {
        throw error::from_sdl();
    }
}

window::window(std::string_view title, dimensions size, window_flags flags)
    : window(window_args{title, size, std::nullopt, flags}) {
}

window::~window() noexcept {
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
}

window::window(window&& other) noexcept
    : m_window{std::exchange(other.m_window, nullptr)}, m_id{std::exchange(other.m_id, window_id{})} {
}

window& window::operator=(window&& other) noexcept {
    if (this != &other) {
        if (m_window) {
            SDL_DestroyWindow(m_window);
        }
        m_window = std::exchange(other.m_window, nullptr);
        m_id = std::exchange(other.m_id, window_id{});
    }
    return *this;
}

void window::show() {
    SDL_Window* win = ensure_handle();
    if (!SDL_ShowWindow(win)) {
        throw error::from_sdl();
    }
}

void window::hide() {
    SDL_Window* win = ensure_handle();
    if (!SDL_HideWindow(win)) {
        throw error::from_sdl();
    }
}

void window::set_title(std::string_view title) {
    SDL_Window* win = ensure_handle();
    const std::string title_str{title};
    if (!SDL_SetWindowTitle(win, title_str.c_str())) {
        throw error::from_sdl();
    }
}

void window::set_size(dimensions size) {
    SDL_Window* win = ensure_handle();
    if (!SDL_SetWindowSize(win, size.width, size.height)) {
        throw error::from_sdl();
    }
}

dimensions window::get_size() const {
    SDL_Window* win = ensure_handle();
    int w, h;
    if (!SDL_GetWindowSize(win, &w, &h)) {
        throw error::from_sdl();
    }
    return {w, h};
}

void window::set_position(position pos) {
    SDL_Window* win = ensure_handle();
    if (!SDL_SetWindowPosition(win, pos.x, pos.y)) {
        throw error::from_sdl();
    }
}

position window::get_position() const {
    SDL_Window* win = ensure_handle();
    int x, y;
    if (!SDL_GetWindowPosition(win, &x, &y)) {
        throw error::from_sdl();
    }
    return {x, y};
}

void window::maximize() {
    SDL_Window* win = ensure_handle();
    if (!SDL_MaximizeWindow(win)) {
        throw error::from_sdl();
    }
}

void window::minimize() {
    SDL_Window* win = ensure_handle();
    if (!SDL_MinimizeWindow(win)) {
        throw error::from_sdl();
    }
}

void window::restore() {
    SDL_Window* win = ensure_handle();
    if (!SDL_RestoreWindow(win)) {
        throw error::from_sdl();
    }
}

void window::raise() {
    SDL_Window* win = ensure_handle();
    if (!SDL_RaiseWindow(win)) {
        throw error::from_sdl();
    }
}

void window::set_fullscreen(bool enabled) {
    SDL_Window* win = ensure_handle();
    if (!SDL_SetWindowFullscreen(win, enabled)) {
        throw error::from_sdl();
    }
}

void window::set_borderless(bool borderless) {
    SDL_Window* win = ensure_handle();
    if (!SDL_SetWindowBordered(win, !borderless)) {
        throw error::from_sdl();
    }
}

void window::set_resizable(bool resizable) {
    SDL_Window* win = ensure_handle();
    if (!SDL_SetWindowResizable(win, resizable)) {
        throw error::from_sdl();
    }
}

void window::set_minimum_size(dimensions size) {
    SDL_Window* win = ensure_handle();
    if (!SDL_SetWindowMinimumSize(win, size.width, size.height)) {
        throw error::from_sdl();
    }
}

void window::set_maximum_size(dimensions size) {
    SDL_Window* win = ensure_handle();
    if (!SDL_SetWindowMaximumSize(win, size.width, size.height)) {
        throw error::from_sdl();
    }
}

dimensions window::get_minimum_size() const {
    SDL_Window* win = ensure_handle();
    int w = 0, h = 0;
    if (!SDL_GetWindowMinimumSize(win, &w, &h)) {
        throw error::from_sdl();
    }
    return {w, h};
}

dimensions window::get_maximum_size() const {
    SDL_Window* win = ensure_handle();
    int w = 0, h = 0;
    if (!SDL_GetWindowMaximumSize(win, &w, &h)) {
        throw error::from_sdl();
    }
    return {w, h};
}

void window::set_opacity(float opacity) {
    SDL_Window* win = ensure_handle();
    if (!SDL_SetWindowOpacity(win, opacity)) {
        throw error::from_sdl();
    }
}

float window::get_opacity() const {
    SDL_Window* win = ensure_handle();
    const float opacity = SDL_GetWindowOpacity(win);
    if (opacity < 0.0f) {
        throw error::from_sdl();
    }
    return opacity;
}

void window::set_mouse_grab(bool grab) {
    SDL_Window* win = ensure_handle();
    if (!SDL_SetWindowMouseGrab(win, grab)) {
        throw error::from_sdl();
    }
}

void window::set_keyboard_grab(bool grab) {
    SDL_Window* win = ensure_handle();
    if (!SDL_SetWindowKeyboardGrab(win, grab)) {
        throw error::from_sdl();
    }
}

bool window::is_valid() const noexcept {
    return m_window != nullptr && m_id.is_valid();
}

SDL_Window* window::ensure_handle() const {
    if (!m_window) {
        throw error(std::source_location::current(), "Window handle is null");
    }
    return m_window;
}

}  // namespace laya
