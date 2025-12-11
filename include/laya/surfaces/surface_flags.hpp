/// @file surface_flags.hpp
/// @brief Surface creation flags for surface operations
/// @date 2025-12-10

#pragma once

#include "../bitmask.hpp"

namespace laya {

/// Surface creation and operation flags
enum class surface_flags : unsigned {
    none = 0,
    preallocated = 0x00000001,  ///< Surface uses preallocated memory
    rle_optimized = 0x00000002  ///< Surface is RLE optimized for fast blitting
};

/// Enable bitmask operations for surface_flags
template <>
struct enable_bitmask_operators<surface_flags> : std::true_type {};

}  // namespace laya
