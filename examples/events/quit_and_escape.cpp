#include <iostream>
#include <variant>

#include <laya/laya.hpp>

#include "example_base.hpp"

int main(int argc, char* argv[]) {
    auto settings = laya_examples::parse_settings(argc, argv);

    laya::context ctx{laya::subsystem::video};
    laya::window window{"quit_and_escape", {640, 480}, laya::window_flags::shown};

    bool running = true;
    int iterations = 0;

    std::cout << "Close the window or press ESC to exit\n";

    while (running) {
        for (const auto& event : laya::events_range()) {
            if (std::holds_alternative<laya::quit_event>(event)) {
                std::cout << "Quit event received\n";
                running = false;
            } else if (const auto* key = std::get_if<laya::key_event>(&event)) {
                if (key->key_state == laya::key_event::state::pressed && key->scancode == 41) {
                    std::cout << "ESC pressed\n";
                    running = false;
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
