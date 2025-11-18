/// @file window_id.hpp
/// @date 2025-10-02

#pragma once

#include <cstdint>

namespace laya {

/// Strong type wrapper for SDL window IDs
/// @note Provides type safety and prevents accidental integer conversions
class window_id {
public:
    /// Default constructor - invalid ID
    constexpr window_id() noexcept;

    /// Construct from raw SDL window ID
    explicit constexpr window_id(std::uint32_t id) noexcept;

    /// Get the underlying ID value
    [[nodiscard]] constexpr std::uint32_t value() const noexcept;

    /// Check if the ID is valid (non-zero)
    [[nodiscard]] constexpr bool is_valid() const noexcept;

    /// Equality comparison
    [[nodiscard]] constexpr bool operator==(const window_id& other) const noexcept;

    /// Inequality comparison
    [[nodiscard]] constexpr bool operator!=(const window_id& other) const noexcept;

    /// Less-than comparison (for ordered containers)
    [[nodiscard]] constexpr bool operator<(const window_id& other) const noexcept;

private:
    std::uint32_t m_id;
};

// ============================================================================
// window_id inline implementations
// ============================================================================

constexpr window_id::window_id() noexcept : m_id{0} {
}

constexpr window_id::window_id(std::uint32_t id) noexcept : m_id{id} {
}

constexpr std::uint32_t window_id::value() const noexcept {
    return m_id;
}

constexpr bool window_id::is_valid() const noexcept {
    return m_id != 0u;
}

constexpr bool window_id::operator==(const window_id& other) const noexcept {
    return m_id == other.m_id;
}

constexpr bool window_id::operator!=(const window_id& other) const noexcept {
    return m_id != other.m_id;
}

constexpr bool window_id::operator<(const window_id& other) const noexcept {
    return m_id < other.m_id;
}

}  // namespace laya
