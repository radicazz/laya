/// @file subsystems.hpp
/// @date 2025-09-30

#pragma once

#include "bitmask.hpp"

namespace laya {

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

void create_subsystem(subsystem system);
void destroy_subsystem(subsystem system) noexcept;
void destroy() noexcept;

template <>
struct enable_bitmask_operators<subsystem> : std::true_type {};

struct context {
    explicit context(subsystem system);
    ~context() noexcept;

    context(const context&) = delete;
    context& operator=(const context&) = delete;
    context(context&&) = delete;
    context& operator=(context&&) = delete;

private:
    subsystem m_system;
};

}  // namespace laya
