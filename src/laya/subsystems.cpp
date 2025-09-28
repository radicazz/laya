#include <laya/laya.hpp>

#include <stdexcept>
#include <string>  // For clang builds.
#include <string_view>

#include <SDL3/SDL.h>

namespace laya::sdl {

std::string_view platform_view() {
    return SDL_GetPlatform();
}

std::string_view error_view() {
    return SDL_GetError();
}

void error_clear() noexcept {
    SDL_ClearError();
}

void subsystem_create(subsystem system) {
    if (SDL_InitSubSystem(enum_underlying_type(system)) == false) {
        throw std::runtime_error("Failed to initialize subsystem: " + std::string(sdl::error_view()));
    }
}

void subsystem_destroy(subsystem system) noexcept {
    SDL_QuitSubSystem(enum_underlying_type(system));
}

}  // namespace laya::sdl

namespace laya {

void create(subsystem system) {
    sdl::subsystem_create(system);
}

void destroy() noexcept {
    SDL_Quit();
}

context::context(subsystem system) {
    create(system);
}

context::~context() noexcept {
    destroy();
}

}  // namespace laya
