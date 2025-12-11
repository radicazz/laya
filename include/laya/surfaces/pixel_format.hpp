/// @file pixel_format.hpp
/// @brief Pixel format enumeration for surface operations
/// @date 2025-12-10

#pragma once

#include <cstdint>

namespace laya {

/// Pixel format enumeration for surfaces and textures
enum class pixel_format : std::uint32_t {
    unknown = 0,
    rgba32 = 0x16462004,  ///< SDL_PIXELFORMAT_RGBA32 - 32-bit RGBA format
    argb32 = 0x16362004,  ///< SDL_PIXELFORMAT_ARGB32 - 32-bit ARGB format
    bgra32 = 0x16762004,  ///< SDL_PIXELFORMAT_BGRA32 - 32-bit BGRA format
    abgr32 = 0x16662004,  ///< SDL_PIXELFORMAT_ABGR32 - 32-bit ABGR format
    rgb24 = 0x17101803,   ///< SDL_PIXELFORMAT_RGB24 - 24-bit RGB format
    bgr24 = 0x17401803    ///< SDL_PIXELFORMAT_BGR24 - 24-bit BGR format
};

}  // namespace laya
