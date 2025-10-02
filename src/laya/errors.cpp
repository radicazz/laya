#include <laya/errors.hpp>
#include <SDL3/SDL.h>

namespace laya {

error error::from_sdl(const std::source_location& location) {
    return error(location, "SDL Error: {}", SDL_GetError());
}

}  // namespace laya
