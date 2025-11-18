/// @file log_category.hpp
/// @brief Type-safe wrapper for SDL3 log categories
/// @date 2025-11-18

#pragma once

namespace laya {

/// Log category types
enum class log_category {
    application,  ///< Application-specific logs
    error,        ///< Error logs
    assert_cat,   ///< Assertion logs
    system,       ///< System logs
    audio,        ///< Audio subsystem logs
    video,        ///< Video subsystem logs
    render,       ///< Render subsystem logs
    input,        ///< Input subsystem logs
    test,         ///< Test logs
    gpu,          ///< GPU subsystem logs
    custom        ///< Custom user-defined category
};

}  // namespace laya
