/// @file log.hpp
/// @brief Modern C++ wrapper for SDL3 logging with std::format support
/// @date 2025-11-18

#pragma once

#include <format>
#include <source_location>

#include "log_category.hpp"
#include "log_priority.hpp"

namespace laya {

// ============================================================================
// Simple logging API (defaults to application category)
// ============================================================================

/// Log message with info priority
template <class... Args>
void log(std::format_string<Args...> fmt, Args&&... args);

/// Log trace message (very verbose debug info)
template <class... Args>
void log_trace(std::format_string<Args...> fmt, Args&&... args);

/// Log verbose message (verbose debug info)
template <class... Args>
void log_verbose(std::format_string<Args...> fmt, Args&&... args);

/// Log debug message
template <class... Args>
void log_debug(std::format_string<Args...> fmt, Args&&... args);

/// Log info message
template <class... Args>
void log_info(std::format_string<Args...> fmt, Args&&... args);

/// Log warning message
template <class... Args>
void log_warn(std::format_string<Args...> fmt, Args&&... args);

/// Log error message (automatically includes source location)
template <class... Args>
void log_error(const std::source_location& loc, std::format_string<Args...> fmt, Args&&... args);

/// Log error message with current source location
template <class... Args>
void log_error(std::format_string<Args...> fmt, Args&&... args);

/// Log critical error message (automatically includes source location)
template <class... Args>
void log_critical(const std::source_location& loc, std::format_string<Args...> fmt, Args&&... args);

/// Log critical error message with current source location
template <class... Args>
void log_critical(std::format_string<Args...> fmt, Args&&... args);

// ============================================================================
// Advanced logging API (with category/priority control)
// ============================================================================

/// Log message with specific category and priority
template <class... Args>
void log(log_category category, log_priority priority, std::format_string<Args...> fmt, Args&&... args);

/// Log message with specific category, priority, and source location
template <class... Args>
void log(log_category category, log_priority priority, const std::source_location& loc, std::format_string<Args...> fmt,
         Args&&... args);

// ============================================================================
// Priority management
// ============================================================================

/// Set minimum priority for a specific category
void set_log_priority(log_category category, log_priority priority);

/// Get current minimum priority for a category
[[nodiscard]] log_priority get_log_priority(log_category category);

/// Set priority for all categories
void set_all_log_priorities(log_priority priority);

/// Reset all log priorities to SDL defaults
void reset_log_priorities() noexcept;

// ============================================================================
// Console color support (cross-platform)
// ============================================================================

/// Enable colored console output (platform-dependent)
/// @note Only works on platforms that support ANSI colors or Windows console
/// @return true if colors were enabled, false if not supported
bool enable_log_colors() noexcept;

/// Disable colored console output
void disable_log_colors() noexcept;

/// Check if colored output is currently enabled
[[nodiscard]] bool are_log_colors_enabled() noexcept;

// ============================================================================
// RAII priority guard
// ============================================================================

/// RAII guard for temporary log priority changes
class log_priority_guard {
public:
    /// Set temporary priority for a category
    log_priority_guard(log_category category, log_priority new_priority);

    /// Restore previous priority
    ~log_priority_guard() noexcept;

    // Non-copyable, non-movable
    log_priority_guard(const log_priority_guard&) = delete;
    log_priority_guard& operator=(const log_priority_guard&) = delete;
    log_priority_guard(log_priority_guard&&) = delete;
    log_priority_guard& operator=(log_priority_guard&&) = delete;

private:
    log_category m_category;
    log_priority m_old_priority;
};

/// Create RAII guard for temporary priority change
[[nodiscard]] inline log_priority_guard with_log_priority(log_category category, log_priority priority) {
    return log_priority_guard{category, priority};
}

// ============================================================================
// Template implementations
// ============================================================================

namespace detail {
// Internal helper functions (implemented in log.cpp)
void log_message(log_category category, log_priority priority, std::string_view message);
void log_message_with_location(log_category category, log_priority priority, std::string_view message,
                               const std::source_location& loc);
}  // namespace detail

template <class... Args>
inline void log(std::format_string<Args...> fmt, Args&&... args) {
    detail::log_message(log_category::application, log_priority::info, std::format(fmt, std::forward<Args>(args)...));
}

template <class... Args>
inline void log_trace(std::format_string<Args...> fmt, Args&&... args) {
    detail::log_message(log_category::application, log_priority::trace, std::format(fmt, std::forward<Args>(args)...));
}

template <class... Args>
inline void log_verbose(std::format_string<Args...> fmt, Args&&... args) {
    detail::log_message(log_category::application, log_priority::verbose,
                        std::format(fmt, std::forward<Args>(args)...));
}

template <class... Args>
inline void log_debug(std::format_string<Args...> fmt, Args&&... args) {
    detail::log_message(log_category::application, log_priority::debug, std::format(fmt, std::forward<Args>(args)...));
}

template <class... Args>
inline void log_info(std::format_string<Args...> fmt, Args&&... args) {
    detail::log_message(log_category::application, log_priority::info, std::format(fmt, std::forward<Args>(args)...));
}

template <class... Args>
inline void log_warn(std::format_string<Args...> fmt, Args&&... args) {
    detail::log_message(log_category::application, log_priority::warn, std::format(fmt, std::forward<Args>(args)...));
}

template <class... Args>
inline void log_error(const std::source_location& loc, std::format_string<Args...> fmt, Args&&... args) {
    detail::log_message_with_location(log_category::application, log_priority::error,
                                      std::format(fmt, std::forward<Args>(args)...), loc);
}

template <class... Args>
inline void log_error(std::format_string<Args...> fmt, Args&&... args) {
    log_error(std::source_location::current(), fmt, std::forward<Args>(args)...);
}

template <class... Args>
inline void log_critical(const std::source_location& loc, std::format_string<Args...> fmt, Args&&... args) {
    detail::log_message_with_location(log_category::application, log_priority::critical,
                                      std::format(fmt, std::forward<Args>(args)...), loc);
}

template <class... Args>
inline void log_critical(std::format_string<Args...> fmt, Args&&... args) {
    log_critical(std::source_location::current(), fmt, std::forward<Args>(args)...);
}

template <class... Args>
inline void log(log_category category, log_priority priority, std::format_string<Args...> fmt, Args&&... args) {
    detail::log_message(category, priority, std::format(fmt, std::forward<Args>(args)...));
}

template <class... Args>
inline void log(log_category category, log_priority priority, const std::source_location& loc,
                std::format_string<Args...> fmt, Args&&... args) {
    detail::log_message_with_location(category, priority, std::format(fmt, std::forward<Args>(args)...), loc);
}

}  // namespace laya
