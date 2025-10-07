/// @file errors.hpp
/// @date 2025-10-01
/// @todo - Add `sdl_error` sub-class instead of static method?

#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <format>
#include <source_location>

namespace laya {

/// Base exception class for all laya errors
class error : public std::runtime_error {
public:
    /// Construct error with simple message
    explicit error(std::string_view message) : std::runtime_error(std::string(message)) {
    }

    /// Construct error with formatted message
    template <typename... Args>
    error(std::format_string<Args...> fmt, Args&&... args)
        : std::runtime_error(std::format(fmt, std::forward<Args>(args)...)) {
    }

    /// Construct error with message and source location
    error(std::string_view message, const std::source_location& location = std::source_location::current())
        : std::runtime_error(format_with_location(message, location)) {
    }

    /// Construct error with formatted message and source location
    template <typename... Args>
    error(const std::source_location& location, std::format_string<Args...> fmt, Args&&... args)
        : std::runtime_error(format_with_location(std::format(fmt, std::forward<Args>(args)...), location)) {
    }

    /// Create error from SDL error with source location
    static error from_sdl(const std::source_location& location = std::source_location::current());

private:
    static std::string format_with_location(std::string_view message, const std::source_location& location) {
        return std::format("[{}:{}:{}] {}", location.file_name(), location.line(), location.function_name(), message);
    }
};

}  // namespace laya
