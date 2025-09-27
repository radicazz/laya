#include <laya/init.hpp>
#include <stdexcept>
#include <SDL3/SDL.h>

namespace laya {

    void init() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
            throw std::runtime_error("Failed to initialize SDL3: " + std::string(SDL_GetError()));
        }
    }

    void quit() noexcept {
        SDL_Quit();
    }

}  // namespace laya
