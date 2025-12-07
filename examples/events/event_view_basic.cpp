#include <iostream>
#include <variant>

#include <laya/laya.hpp>

#include "example_base.hpp"

int main(int argc, char* argv[]) {
    auto settings = laya_examples::parse_settings(argc, argv);

    laya::context ctx{laya::subsystem::video};
    laya::window window{"event_view_basic", {640, 480}, laya::window_flags::shown};

    bool running = true;
    int iterations = 0;

    while (running) {
        for (const auto& event : laya::events_view()) {
            std::visit(
                [&](const auto& e) {
                    using T = std::decay_t<decltype(e)>;

                    if constexpr (std::is_same_v<T, laya::quit_event>) {
                        std::cout << "Quit event\n";
                        running = false;
                    } else if constexpr (std::is_same_v<T, laya::key_event>) {
                        if (e.key_state == laya::key_event::state::pressed) {
                            std::cout << "Key pressed: scancode=" << e.scancode << "\n";
                            if (e.scancode == 41) {  // ESC
                                running = false;
                            }
                        }
                    } else if constexpr (std::is_same_v<T, laya::mouse_button_event>) {
                        std::cout << "Mouse button at (" << e.x << ", " << e.y << ")\n";
                    }
                },
                event);
        }

        if (laya_examples::tick_ci(settings, iterations)) {
            running = false;
        }

        laya_examples::sleep_frame();
    }

    return 0;
}
