// Shared helpers for small, focused examples
#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

namespace laya_examples {

struct settings {
    bool ci_mode{false};
    int ci_iterations{5};
};

inline settings parse_settings(int argc, char* argv[], int default_iterations = 5) {
    settings result;
    result.ci_iterations = default_iterations;

    for (int i = 1; i < argc; ++i) {
        std::string arg{argv[i]};
        if (arg == "--ci") {
            result.ci_mode = true;
            std::cout << "Running in CI mode (non-interactive)\n";
        }
    }

    return result;
}

inline bool tick_ci(const settings& cfg, int& iterations) {
    if (!cfg.ci_mode) {
        return false;
    }

    ++iterations;
    if (iterations >= cfg.ci_iterations) {
        std::cout << "CI mode: completed " << iterations << " iterations\n";
        return true;
    }

    return false;
}

inline void sleep_frame(std::chrono::milliseconds duration = std::chrono::milliseconds{16}) {
    std::this_thread::sleep_for(duration);
}

}  // namespace laya_examples
