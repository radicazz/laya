#include <iostream>
#include <variant>

#include <laya/laya.hpp>

#include "example_base.hpp"

int main(int argc, char* argv[]) {
    auto settings = laya_examples::parse_settings(argc, argv);

    try {
        laya::context ctx{laya::subsystem::video};
        laya::window win{"render_quit_with_events", {640, 480}, laya::window_flags::shown};
        laya::renderer renderer{win};

        bool running = true;
        int iterations = 0;

        std::cout << "Close the window or press ESC to exit\n";

        while (running) {
            for (const auto& event : laya::events_range()) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    running = false;
                } else if (const auto* key = std::get_if<laya::key_event>(&event)) {
                    if (key->key_state == laya::key_event::state::pressed && key->scancode == 41) {
                        running = false;
                    }
                }
            }

            renderer.set_draw_color(laya::colors::black);
            renderer.clear();
            renderer.present();

            if (laya_examples::tick_ci(settings, iterations)) {
                running = false;
            }

            laya_examples::sleep_frame();
        }
    } catch (const laya::error& e) {
        if (settings.ci_mode) {
            std::cout << "Skipping render_quit_with_events in CI: " << e.what() << "\n";
            return 0;
        }
        std::cerr << "Laya error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        if (settings.ci_mode) {
            std::cout << "Skipping render_quit_with_events in CI: " << e.what() << "\n";
            return 0;
        }
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
