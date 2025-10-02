#include <laya/laya.hpp>
#include <iostream>
#include <variant>
#include <string>
#include <thread>
#include <chrono>

namespace {

constexpr int ci_iterations = 5;  // Number of iterations in CI mode

}  // anonymous namespace

/// @brief Demonstrates event_view - zero-allocation lazy event polling
/// Events are polled one at a time, no memory allocation
void demo_event_view(laya::window& window, bool ci_mode) {
    std::cout << "\n=== event_view Demo (Zero-Allocation) ===" << std::endl;

    if (!ci_mode) {
        std::cout << "Click the window or press a key (ESC to continue)..." << std::endl;
    }

    bool running = true;
    int iterations = 0;

    while (running) {
        // poll_events_view() returns an event_view - lazy iteration, no allocations
        for (const auto& event : laya::poll_events_view()) {
            std::visit(
                [&](const auto& e) {
                    using T = std::decay_t<decltype(e)>;

                    if constexpr (std::is_same_v<T, laya::quit_event>) {
                        std::cout << "Quit event" << std::endl;
                        running = false;
                    } else if constexpr (std::is_same_v<T, laya::key_event>) {
                        if (e.key_state == laya::key_event::state::pressed) {
                            std::cout << "Key pressed: scancode=" << e.scancode << std::endl;
                            if (e.scancode == 41) {  // ESC
                                running = false;
                            }
                        }
                    } else if constexpr (std::is_same_v<T, laya::mouse_button_event>) {
                        std::cout << "Mouse button at (" << e.x << ", " << e.y << ")" << std::endl;
                    }
                },
                event);
        }

        // CI mode: exit after fixed iterations
        if (ci_mode) {
            ++iterations;
            if (iterations >= ci_iterations) {
                std::cout << "CI mode: completed " << iterations << " iterations" << std::endl;
                running = false;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

/// @brief Demonstrates event_range - events stored in vector for multi-pass iteration
/// All events are polled into memory, can be iterated multiple times
void demo_event_range(laya::window& window, bool ci_mode) {
    std::cout << "\n=== event_range Demo (Stored Events) ===" << std::endl;

    if (!ci_mode) {
        std::cout << "Click the window or press keys (ESC to exit)..." << std::endl;
    }

    bool running = true;
    int iterations = 0;

    while (running) {
        // poll_events_range() returns an event_range - all events stored in vector
        auto events = laya::poll_events_range();

        if (!events.empty()) {
            std::cout << "\nProcessed " << events.size() << " event(s):" << std::endl;

            // First pass - handle quit/ESC
            for (const auto& event : events) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    running = false;
                } else if (const auto* key = std::get_if<laya::key_event>(&event)) {
                    if (key->scancode == 41 && key->key_state == laya::key_event::state::pressed) {
                        running = false;
                    }
                }
            }

            // Second pass - log all events (demonstrates multi-pass capability)
            for (const auto& event : events) {
                std::visit(
                    [](const auto& e) {
                        using T = std::decay_t<decltype(e)>;
                        if constexpr (std::is_same_v<T, laya::key_event>) {
                            if (e.key_state == laya::key_event::state::pressed) {
                                std::cout << "  - Key: scancode=" << e.scancode << std::endl;
                            }
                        } else if constexpr (std::is_same_v<T, laya::mouse_button_event>) {
                            std::cout << "  - Mouse button at (" << e.x << ", " << e.y << ")" << std::endl;
                        } else if constexpr (std::is_same_v<T, laya::mouse_motion_event>) {
                            std::cout << "  - Mouse motion: (" << e.x << ", " << e.y << ")" << std::endl;
                        } else if constexpr (std::is_same_v<T, laya::window_event>) {
                            std::cout << "  - Window event: type=" << static_cast<int>(e.event_type) << std::endl;
                        }
                    },
                    event);
            }
        }

        // CI mode: exit after fixed iterations
        if (ci_mode) {
            ++iterations;
            if (iterations >= ci_iterations) {
                std::cout << "CI mode: completed " << iterations << " iterations" << std::endl;
                running = false;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

int main(int argc, char* argv[]) {
    try {
        // Check for CI mode flag
        bool ci_mode = false;
        if (argc > 1 && std::string(argv[1]) == "--ci") {
            ci_mode = true;
            std::cout << "Running in CI mode (non-interactive)" << std::endl;
        }

        // Initialize SDL with video subsystem (events are implied)
        laya::context ctx(laya::subsystem::video);
        laya::window window("Event Polling Demo", {800, 600}, laya::window_flags::resizable);

        std::cout << "Window created with ID: " << window.id().value() << std::endl;
        std::cout << "This demo shows two event polling approaches:" << std::endl;
        std::cout << "1. event_view - Zero allocation, lazy iteration" << std::endl;
        std::cout << "2. event_range - Stored events, multi-pass iteration" << std::endl;

        // Demo 1: event_view (zero-allocation lazy polling)
        demo_event_view(window, ci_mode);

        // Demo 2: event_range (stored events for multi-pass)
        demo_event_range(window, ci_mode);

        std::cout << "\nDemo completed successfully!" << std::endl;
        return 0;

    } catch (const laya::error& e) {
        std::cerr << "SDL error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
