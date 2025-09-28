/// @file bitmask.hpp
/// @date 2025-09-28

#pragma once

#include <type_traits>

namespace laya {

template <class T>
struct enable_bitmask_operators : std::false_type {};

template <class T>
constexpr bool enable_bitmask_operators_v = enable_bitmask_operators<T>::value;

/// Enum class (scoped) as opposed to unscoped enum.
template <class T>
concept scoped_enum = std::is_enum_v<T> && !std::is_convertible_v<T, std::underlying_type_t<T>>;

/// Enum of uints that has bitmask operators enabled.
/// \tparam T The enum type to enable bitmask operators for.
template <class T>
concept bitmask_enum =
    scoped_enum<T> && std::is_same_v<std::underlying_type_t<T>, unsigned> && enable_bitmask_operators_v<T>;

constexpr [[nodiscard]]
auto operator|(bitmask_enum auto lhs, bitmask_enum auto rhs) noexcept -> decltype(lhs) {
    using U = std::underlying_type_t<decltype(lhs)>;
    return static_cast<decltype(lhs)>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

constexpr [[nodiscard]]
auto operator&(bitmask_enum auto lhs, bitmask_enum auto rhs) noexcept -> decltype(lhs) {
    using U = std::underlying_type_t<decltype(lhs)>;
    return static_cast<decltype(lhs)>(static_cast<U>(lhs) & static_cast<U>(rhs));
}

constexpr [[nodiscard]]
auto operator^(bitmask_enum auto lhs, bitmask_enum auto rhs) noexcept -> decltype(lhs) {
    using U = std::underlying_type_t<decltype(lhs)>;
    return static_cast<decltype(lhs)>(static_cast<U>(lhs) ^ static_cast<U>(rhs));
}

constexpr [[nodiscard]]
auto operator~(bitmask_enum auto e) noexcept -> decltype(e) {
    using U = std::underlying_type_t<decltype(e)>;
    return static_cast<decltype(e)>(~static_cast<U>(e));
}

constexpr auto operator|=(bitmask_enum auto& lhs, decltype(lhs) rhs) noexcept -> decltype(lhs) {
    lhs = lhs | rhs;
    return lhs;
}

constexpr auto operator&=(bitmask_enum auto& lhs, decltype(lhs) rhs) noexcept -> decltype(lhs) {
    lhs = lhs & rhs;
    return lhs;
}

constexpr auto operator^=(bitmask_enum auto& lhs, decltype(lhs) rhs) noexcept -> decltype(lhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

template <class E>
constexpr [[nodiscard]] auto enum_underlying_type(E e) noexcept -> std::underlying_type_t<E> {
    static_assert(std::is_enum_v<E>, "to_underlying requires an enumeration type");
    return static_cast<std::underlying_type_t<E>>(e);
}

}  // namespace laya
