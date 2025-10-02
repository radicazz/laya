/// @file errors.hpp
/// @date 2025-10-01

#pragma once

#include <stdexcept>

namespace laya {
class error : public std::runtime_error {
public:
    explicit error(const char* message) : std::runtime_error(message) {
    }

    static error from_sdl();
};
}  // namespace laya
