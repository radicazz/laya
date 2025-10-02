/// @file perf_utils.hpp
/// @brief Performance testing utilities
/// @date 2025-10-01

#pragma once

#include <chrono>
#include <format>
#include <string>

namespace laya_perf {

/// @brief Format microseconds as "0.123456s::123.456ms"
/// @param microseconds Time in microseconds
/// @return Formatted string with seconds and milliseconds
inline std::string format_time(double microseconds) {
    double seconds = microseconds / 1'000'000.0;
    double milliseconds = microseconds / 1'000.0;
    return std::format("{:.6f}s::{:.3f}ms", seconds, milliseconds);
}

}  // namespace laya_perf
