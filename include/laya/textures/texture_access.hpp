/// Texture access patterns and scaling modes for SDL3 texture operations.
/// \file texture_access.hpp
/// \date 2025-09-28

#pragma once

#include <laya/bitmask.hpp>

#include <SDL3/SDL.h>

namespace laya {

/// Texture access patterns define how textures can be used.
/// Maps directly to SDL_TextureAccess values for type safety.
enum class texture_access : int {
    /// Texture changes rarely, not lockable.
    static_ = SDL_TEXTUREACCESS_STATIC,

    /// Texture changes frequently, lockable.
    streaming = SDL_TEXTUREACCESS_STREAMING,

    /// Texture can be used as a render target.
    target = SDL_TEXTUREACCESS_TARGET
};

/// Texture scaling modes for rendering operations.
/// Maps directly to SDL_ScaleMode values for type safety.
enum class scale_mode : int {
    /// Nearest pixel sampling.
    nearest = SDL_SCALEMODE_NEAREST,

    /// Linear filtering.
    linear = SDL_SCALEMODE_LINEAR
};

}  // namespace laya
