#include <iostream>
#include <variant>

#include <laya/laya.hpp>

#include "example_base.hpp"

int main(int argc, char* argv[]) {
    auto settings = laya_examples::parse_settings(argc, argv);

    laya::context ctx{laya::subsystem::video};
    laya::window window{"window_event_data", {640, 480}, laya::window_flags::shown};

    bool running = true;
    int iterations = 0;

    while (running) {
        for (const auto& event : laya::events_range()) {
            if (std::holds_alternative<laya::quit_event>(event)) {
                running = false;
                continue;
            }

            if (const auto* key = std::get_if<laya::key_event>(&event)) {
                if (key->key_state == laya::key_event::state::pressed && key->scancode == 41) {
                    running = false;
                }
            }

            if (const auto* win_event = std::get_if<laya::window_event>(&event)) {
                if (auto pos = laya::get_position(*win_event)) {
                    std::cout << "Window moved to (" << pos->x << ", " << pos->y << ")\n";
                } else if (auto size = laya::get_size(*win_event)) {
                    std::cout << "Window resized to " << size->width << "x" << size->height << "\n";
                } else if (auto display = laya::get_display(*win_event)) {
                    std::cout << "Window moved to display " << display->display_index << "\n";
                }
            }
        }

        if (laya_examples::tick_ci(settings, iterations)) {
            running = false;
        }

        laya_examples::sleep_frame();
    }

    return 0;
}
