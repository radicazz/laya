#include <iostream>
#include <variant>
#include <string>
#include <thread>
#include <chrono>

#include <laya/laya.hpp>

namespace {

constexpr int ci_iterations = 5;  // Number of iterations in CI mode

/// Helper to format window event data for display
std::string format_window_event_data(const laya::window_event& event) {
    std::string result;

    // Use std::visit for type-safe pattern matching
    std::visit(
        [&](const auto& data) {
            using T = std::decay_t<decltype(data)>;

            if constexpr (std::is_same_v<T, laya::window_event_data_position>) {
                result = " (position: " + std::to_string(data.x) + ", " + std::to_string(data.y) + ")";
            } else if constexpr (std::is_same_v<T, laya::window_event_data_size>) {
                result = " (size: " + std::to_string(data.width) + "x" + std::to_string(data.height) + ")";
            } else if constexpr (std::is_same_v<T, laya::window_event_data_display>) {
                result = " (display: " + std::to_string(data.display_index) + ")";
            } else if constexpr (std::is_same_v<T, laya::window_event_data_none>) {
                result = "";
            }
        },
        event.data);

    return result;
}

/// Helper to get window event type name
const char* get_event_type_name(laya::window_event_type type) {
    switch (type) {
        case laya::window_event_type::shown:
            return "shown";
        case laya::window_event_type::hidden:
            return "hidden";
        case laya::window_event_type::exposed:
            return "exposed";
        case laya::window_event_type::moved:
            return "moved";
        case laya::window_event_type::resized:
            return "resized";
        case laya::window_event_type::size_changed:
            return "size_changed";
        case laya::window_event_type::minimized:
            return "minimized";
        case laya::window_event_type::maximized:
            return "maximized";
        case laya::window_event_type::restored:
            return "restored";
        case laya::window_event_type::enter:
            return "mouse_enter";
        case laya::window_event_type::leave:
            return "mouse_leave";
        case laya::window_event_type::focus_gained:
            return "focus_gained";
        case laya::window_event_type::focus_lost:
            return "focus_lost";
        case laya::window_event_type::close:
            return "close";
        case laya::window_event_type::take_focus:
            return "take_focus";
        case laya::window_event_type::hit_test:
            return "hit_test";
        case laya::window_event_type::icc_profile_changed:
            return "icc_profile_changed";
        case laya::window_event_type::display_changed:
            return "display_changed";
        default:
            return "unknown";
    }
}

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
        // events_view() returns an event_view - lazy iteration, no allocations
        for (const auto& event : laya::events_view()) {
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
        // events_range() returns an event_range - all events stored in vector
        auto events = laya::events_range();

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
                            std::cout << "  - Window event: " << get_event_type_name(e.event_type)
                                      << format_window_event_data(e) << std::endl;

                            // Alternative: demonstrate helper functions
                            if (auto pos = laya::get_position(e)) {
                                std::cout << "    [Helper] Position: (" << pos->x << ", " << pos->y << ")" << std::endl;
                            } else if (auto size = laya::get_size(e)) {
                                std::cout << "    [Helper] Size: " << size->width << "x" << size->height << std::endl;
                            } else if (auto display = laya::get_display(e)) {
                                std::cout << "    [Helper] Display: " << display->display_index << std::endl;
                            }
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
