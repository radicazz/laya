/// @file log_priority.hpp
/// @brief Type-safe wrapper for SDL3 log priorities
/// @date 2025-11-18

#pragma once

namespace laya {

/// Log priority levels
enum class log_priority {
    trace = 1,  ///< Very verbose debugging information
    verbose,    ///< Verbose debugging information
    debug,      ///< Debug information
    info,       ///< Informational messages
    warn,       ///< Warning messages
    error,      ///< Error messages
    critical    ///< Critical error messages
};

}  // namespace laya
