#include <iostream>
#include <variant>

#include <laya/laya.hpp>

#include "example_base.hpp"

namespace {

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

}  // namespace

int main(int argc, char* argv[]) {
    auto settings = laya_examples::parse_settings(argc, argv);

    laya::context ctx{laya::subsystem::video};
    laya::window window{"event_range_multipass", {640, 480}, laya::window_flags::shown};

    bool running = true;
    int iterations = 0;

    while (running) {
        auto events = laya::events_range();

        // Pass 1: handle quit/ESC
        for (const auto& event : events) {
            if (std::holds_alternative<laya::quit_event>(event)) {
                running = false;
            } else if (const auto* key = std::get_if<laya::key_event>(&event)) {
                if (key->key_state == laya::key_event::state::pressed && key->scancode == 41) {
                    running = false;
                }
            }
        }

        // Pass 2: log everything we saw
        if (!events.empty()) {
            std::cout << "Processed " << events.size() << " event(s):\n";
        }

        for (const auto& event : events) {
            std::visit(
                [](const auto& e) {
                    using T = std::decay_t<decltype(e)>;
                    if constexpr (std::is_same_v<T, laya::key_event>) {
                        if (e.key_state == laya::key_event::state::pressed) {
                            std::cout << "  - Key: scancode=" << e.scancode << "\n";
                        }
                    } else if constexpr (std::is_same_v<T, laya::mouse_button_event>) {
                        std::cout << "  - Mouse button at (" << e.x << ", " << e.y << ")\n";
                    } else if constexpr (std::is_same_v<T, laya::mouse_motion_event>) {
                        std::cout << "  - Mouse motion: (" << e.x << ", " << e.y << ")\n";
                    } else if constexpr (std::is_same_v<T, laya::window_event>) {
                        std::cout << "  - Window event: " << get_event_type_name(e.event_type);

                        if (auto pos = laya::get_position(e)) {
                            std::cout << " -> Position (" << pos->x << ", " << pos->y << ")";
                        } else if (auto size = laya::get_size(e)) {
                            std::cout << " -> Size " << size->width << "x" << size->height;
                        } else if (auto display = laya::get_display(e)) {
                            std::cout << " -> Display " << display->display_index;
                        }

                        std::cout << "\n";
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
