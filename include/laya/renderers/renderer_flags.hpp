/// @file renderer_flags.hpp
/// @brief Renderer creation and configuration flags
/// @date 2025-10-07

#pragma once

#include "../bitmask.hpp"

namespace laya {

/// Renderer creation flags
enum class renderer_flags : unsigned {
    none = 0x00000000,           ///< No specific flags
    software = 0x00000001,       ///< Use software rendering
    accelerated = 0x00000002,    ///< Use hardware acceleration
    present_vsync = 0x00000004,  ///< Enable vertical sync
    target_texture = 0x00000008  ///< Support rendering to texture
};

/// Enable bitmask operations for renderer_flags
template <>
struct enable_bitmask_operators<renderer_flags> : std::true_type {};

}  // namespace laya
