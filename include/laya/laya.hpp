/// Primary header for the laya library.
/// \file laya.hpp
/// \date 2025-09-28

#pragma once

#include <string>

#include "utils/bitmask.hpp"

namespace laya {

/// Flags for initializing various subsystems within the Laya engine.
enum class subsystem : unsigned {
    audio = 0x00000010u,      ///< implies subsystem::events.
    video = 0x00000020u,      ///< implies subsystem::events, must be initialized on main thread.
    joystick = 0x00000200u,   ///< implies subsystem::events.
    haptic = 0x00001000u,     ///< no implied subsystems.
    gamepad = 0x00002000u,    ///< implies subsystem::joystick.
    events = 0x00004000u,     ///< implies subsystem::events.
    sensor = 0x00008000u,     ///< implies subsystem::events.
    camera = 0x00010000u,     ///< implies subsystem::events.
    everything = 0x0000FFFFu  ///< implies subsystem::everything.
};

// Enable bitmask operations for the above subsystem enum.
template <>
struct enable_bitmask_operators<subsystem> : std::true_type {};

namespace sdl {

void subsystem_create(subsystem system);
void subsystem_destroy(subsystem system) noexcept;

std::string_view platform_view();
std::string_view error_view();
void error_clear() noexcept;

}  // namespace sdl

struct context {
    explicit context(subsystem system);
    ~context() noexcept;

    context(const context&) = delete;
    context& operator=(const context&) = delete;
    context(context&&) = delete;
    context& operator=(context&&) = delete;
};

void create(subsystem system);
void destroy() noexcept;
}  // namespace laya
