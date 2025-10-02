#include <utility>

#include <laya/windows/window.hpp>
#include <laya/errors.hpp>
#include <SDL3/SDL.h>

namespace laya {

window::window(const window_args& args)
    : m_window{SDL_CreateWindow(args.title.data(), args.size.width, args.size.height,
                                static_cast<uint32_t>(args.flags))},
      m_id{m_window ? window_id{SDL_GetWindowID(m_window)} : window_id{}} {
    if (!m_window) {
        throw error::from_sdl();
    }
}

window::window(std::string_view title, dimentions size, window_flags flags)
    : m_window{SDL_CreateWindow(title.data(), size.width, size.height, static_cast<uint32_t>(flags))},
      m_id{m_window ? window_id{SDL_GetWindowID(m_window)} : window_id{}} {
    if (!m_window) {
        throw error::from_sdl();
    }
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
    SDL_ShowWindow(m_window);
}

void window::hide() {
    SDL_HideWindow(m_window);
}

void window::set_title(std::string_view title) {
    SDL_SetWindowTitle(m_window, title.data());
}

void window::set_size(dimentions size) {
    SDL_SetWindowSize(m_window, size.width, size.height);
}

dimentions window::get_size() const {
    int w, h;
    SDL_GetWindowSize(m_window, &w, &h);
    return {w, h};
}

void window::set_position(position pos) {
    SDL_SetWindowPosition(m_window, pos.x, pos.y);
}

position window::get_position() const {
    int x, y;
    SDL_GetWindowPosition(m_window, &x, &y);
    return {x, y};
}

void window::maximize() {
    SDL_MaximizeWindow(m_window);
}

void window::minimize() {
    SDL_MinimizeWindow(m_window);
}

void window::restore() {
    SDL_RestoreWindow(m_window);
}

void window::raise() {
    SDL_RaiseWindow(m_window);
}

}  // namespace laya
