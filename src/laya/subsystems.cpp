#include <laya/subsystems.hpp>
#include <laya/errors.hpp>
#include <SDL3/SDL.h>

namespace laya {

namespace {

void set_headless_hints(subsystem system) {
    if ((system & subsystem::video) == subsystem::video) {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    }
    if ((system & subsystem::audio) == subsystem::audio) {
        SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy");
    }
}

}  // namespace

void create_subsystem(subsystem system) {
    set_headless_hints(system);

    auto result = SDL_Init(underlying_type(system));
    if (result != 0) {
        std::fprintf(stderr, "SDL_Init failed for flags 0x%X: %s\n", underlying_type(system), SDL_GetError());
        // Fallback to events-only initialization in headless or constrained environments
        if ((system & subsystem::events) != subsystem::events) {
            if (SDL_Init(SDL_INIT_EVENTS) == 0) {
                return;
            }
        }
        std::fprintf(stderr, "SDL_Init fallback to events-only also failed: %s\n", SDL_GetError());
        return;
    }
}

void destroy_subsystem(subsystem system) noexcept {
    SDL_QuitSubSystem(underlying_type(system));
}

void destroy() noexcept {
    SDL_Quit();
}

context::context(subsystem system) : m_system(system) {
    create_subsystem(system);
}

context::~context() noexcept {
    if (SDL_WasInit(underlying_type(m_system)) != 0) {
        destroy_subsystem(m_system);
    }
}

}  // namespace laya
