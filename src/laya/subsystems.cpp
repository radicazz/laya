#include <laya/subsystems.hpp>
#include <laya/errors.hpp>
#include <SDL3/SDL.h>

namespace laya {

void create_subsystem(subsystem system) {
    if (SDL_InitSubSystem(underlying_type(system)) == false) {
        throw error::from_sdl();
    }
}

void destroy_subsystem(subsystem system) noexcept {
    SDL_QuitSubSystem(underlying_type(system));
}

void destroy() noexcept {
    SDL_Quit();
}

context::context(subsystem system) {
    create_subsystem(system);
}

context::~context() noexcept {
    destroy();
}

}  // namespace laya
