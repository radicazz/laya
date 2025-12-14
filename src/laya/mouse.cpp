/// @file mouse.cpp
/// @date 2025-12-14

#include <laya/input/mouse.hpp>
#include <laya/errors.hpp>
#include <laya/windows/window.hpp>

#include <SDL3/SDL.h>

namespace laya {

namespace {

[[nodiscard]] SDL_Window* ensure_window_handle(const window& win) {
    SDL_Window* handle = win.native_handle();
    if (!handle) {
        throw error(std::source_location::current(), "Window handle is null");
    }
    return handle;
}

[[nodiscard]] SDL_Window* get_focused_window() {
    if (SDL_Window* mouse_focus = SDL_GetMouseFocus(); mouse_focus) {
        return mouse_focus;
    }
    return SDL_GetKeyboardFocus();
}

}  // namespace

point get_mouse_position() {
    float x, y;
    SDL_GetMouseState(&x, &y);
    return {static_cast<int>(x), static_cast<int>(y)};
}

point get_global_mouse_position() {
    float x, y;
    SDL_GetGlobalMouseState(&x, &y);
    return {static_cast<int>(x), static_cast<int>(y)};
}

point get_mouse_position(const window& win) {
    SDL_Window* target = ensure_window_handle(win);

    // If this is the focused window, SDL already provides window-relative coordinates.
    if (SDL_GetMouseFocus() == target) {
        float x, y;
        SDL_GetMouseState(&x, &y);
        return {static_cast<int>(x), static_cast<int>(y)};
    }

    // Otherwise, compute window-relative coordinates from desktop position.
    float gx, gy;
    SDL_GetGlobalMouseState(&gx, &gy);

    int wx = 0, wy = 0;
    if (!SDL_GetWindowPosition(target, &wx, &wy)) {
        throw error::from_sdl();
    }

    return {static_cast<int>(gx) - wx, static_cast<int>(gy) - wy};
}

mouse_button_mask get_mouse_state() {
    const auto state = SDL_GetMouseState(nullptr, nullptr);
    return static_cast<mouse_button_mask>(state);
}

mouse_button_mask get_mouse_state(int& x, int& y) {
    float fx, fy;
    const auto state = SDL_GetMouseState(&fx, &fy);
    x = static_cast<int>(fx);
    y = static_cast<int>(fy);
    return static_cast<mouse_button_mask>(state);
}

bool is_mouse_button_pressed(mouse_button button) {
    const auto state = get_mouse_state();
    const auto mask = static_cast<mouse_button_mask>(1u << (static_cast<int>(button) - 1));
    const auto state_value = static_cast<std::uint32_t>(state);
    const auto mask_value = static_cast<std::uint32_t>(mask);
    return (state_value & mask_value) != 0;
}

mouse_button_mask get_relative_mouse_state(int& x, int& y) {
    float fx, fy;
    const auto state = SDL_GetRelativeMouseState(&fx, &fy);
    x = static_cast<int>(fx);
    y = static_cast<int>(fy);
    return static_cast<mouse_button_mask>(state);
}

void warp_mouse_in_window(window& win, point pos) {
    SDL_WarpMouseInWindow(win.native_handle(), static_cast<float>(pos.x), static_cast<float>(pos.y));
}

void warp_mouse_global(point pos) {
    if (!SDL_WarpMouseGlobal(static_cast<float>(pos.x), static_cast<float>(pos.y))) {
        throw error::from_sdl();
    }
}

void set_relative_mouse_mode(bool enabled) {
    SDL_Window* focus = get_focused_window();
    if (!focus) {
        throw error(std::source_location::current(), "No window has focus");
    }
    if (!SDL_SetWindowRelativeMouseMode(focus, enabled)) {
        throw error::from_sdl();
    }
}

bool is_relative_mouse_mode() {
    SDL_Window* focus = get_focused_window();
    if (!focus) {
        return false;
    }
    return SDL_GetWindowRelativeMouseMode(focus);
}

void capture_mouse(bool enabled) {
    if (!SDL_CaptureMouse(enabled)) {
        throw error::from_sdl();
    }
}

void show_cursor(bool visible) {
    if (visible) {
        SDL_ShowCursor();
    } else {
        SDL_HideCursor();
    }
}

bool is_cursor_visible() {
    return SDL_CursorVisible();
}

// Cursor management
static SDL_Cursor* create_system_cursor(system_cursor cursor) {
    SDL_SystemCursor sdl_cursor;
    switch (cursor) {
        case system_cursor::arrow:
            sdl_cursor = SDL_SYSTEM_CURSOR_DEFAULT;
            break;
        case system_cursor::ibeam:
            sdl_cursor = SDL_SYSTEM_CURSOR_TEXT;
            break;
        case system_cursor::wait:
            sdl_cursor = SDL_SYSTEM_CURSOR_WAIT;
            break;
        case system_cursor::crosshair:
            sdl_cursor = SDL_SYSTEM_CURSOR_CROSSHAIR;
            break;
        case system_cursor::wait_arrow:
            sdl_cursor = SDL_SYSTEM_CURSOR_PROGRESS;
            break;
        case system_cursor::size_nwse:
            sdl_cursor = SDL_SYSTEM_CURSOR_NWSE_RESIZE;
            break;
        case system_cursor::size_nesw:
            sdl_cursor = SDL_SYSTEM_CURSOR_NESW_RESIZE;
            break;
        case system_cursor::size_we:
            sdl_cursor = SDL_SYSTEM_CURSOR_EW_RESIZE;
            break;
        case system_cursor::size_ns:
            sdl_cursor = SDL_SYSTEM_CURSOR_NS_RESIZE;
            break;
        case system_cursor::size_all:
            sdl_cursor = SDL_SYSTEM_CURSOR_MOVE;
            break;
        case system_cursor::no:
            sdl_cursor = SDL_SYSTEM_CURSOR_NOT_ALLOWED;
            break;
        case system_cursor::hand:
            sdl_cursor = SDL_SYSTEM_CURSOR_POINTER;
            break;
        default:
            sdl_cursor = SDL_SYSTEM_CURSOR_DEFAULT;
    }
    return SDL_CreateSystemCursor(sdl_cursor);
}

void set_cursor(system_cursor cursor) {
    SDL_Cursor* sdl_cursor = create_system_cursor(cursor);
    if (!sdl_cursor) {
        throw error::from_sdl();
    }
    SDL_SetCursor(sdl_cursor);
}

void reset_cursor() {
    set_cursor(system_cursor::arrow);
}

// RAII cursor guard
cursor_guard::cursor_guard(system_cursor temp_cursor) : m_previous_cursor(SDL_GetCursor()) {
    set_cursor(temp_cursor);
}

cursor_guard::~cursor_guard() noexcept {
    if (m_previous_cursor) {
        SDL_SetCursor(static_cast<SDL_Cursor*>(m_previous_cursor));
    }
}

}  // namespace laya
