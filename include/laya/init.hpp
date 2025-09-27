#pragma once

namespace laya {

    /// Initialize the laya library and SDL3 subsystems
    /// @throws std::runtime_error if initialization fails
    void init();

    /// Shutdown the laya library and SDL3 subsystems
    void quit() noexcept;

}  // namespace laya
