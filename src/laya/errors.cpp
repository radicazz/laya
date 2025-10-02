#include <laya/errors.hpp>
#include <SDL3/SDL.h>

namespace laya {
error error::from_sdl() {
    return error(SDL_GetError());
}
}  // namespace laya
