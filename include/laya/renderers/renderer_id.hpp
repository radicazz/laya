/// @file renderer_id.hpp
/// @brief Strong type wrapper for renderer identifiers
/// @date 2025-10-07

#pragma once

#include <cstdint>

namespace laya {

/// Strong type wrapper for SDL renderer IDs
/// @note Provides type safety and prevents accidental integer conversions
class renderer_id {
public:
    /// Default constructor - invalid ID
    constexpr renderer_id() noexcept;

    /// Construct from raw SDL renderer ID
    explicit constexpr renderer_id(std::uint32_t id) noexcept;

    /// Get the underlying ID value
    [[nodiscard]] constexpr std::uint32_t value() const noexcept;

    /// Check if the ID is valid (non-zero)
    [[nodiscard]] constexpr bool is_valid() const noexcept;

    /// Equality comparison
    [[nodiscard]] constexpr bool operator==(const renderer_id& other) const noexcept;

    /// Inequality comparison
    [[nodiscard]] constexpr bool operator!=(const renderer_id& other) const noexcept;

    /// Less-than comparison (for ordered containers)
    [[nodiscard]] constexpr bool operator<(const renderer_id& other) const noexcept;

private:
    std::uint32_t m_id;
};

// ============================================================================
// renderer_id inline implementations
// ============================================================================

constexpr renderer_id::renderer_id() noexcept : m_id{0} {
}

constexpr renderer_id::renderer_id(std::uint32_t id) noexcept : m_id{id} {
}

constexpr std::uint32_t renderer_id::value() const noexcept {
    return m_id;
}

constexpr bool renderer_id::is_valid() const noexcept {
    return m_id != 0u;
}

constexpr bool renderer_id::operator==(const renderer_id& other) const noexcept {
    return m_id == other.m_id;
}

constexpr bool renderer_id::operator!=(const renderer_id& other) const noexcept {
    return m_id != other.m_id;
}

constexpr bool renderer_id::operator<(const renderer_id& other) const noexcept {
    return m_id < other.m_id;
}

}  // namespace laya
