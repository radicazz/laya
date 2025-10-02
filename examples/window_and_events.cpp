#include <laya/laya.hpp>
#include <iostream>
#include <variant>
#include <string>
#include <thread>
#include <chrono>

int main() {
    try {
        // Initialize SDL with video subsystem (events are implied)
        // The context RAII object will clean up on exit
        laya::context ctx(laya::subsystem::video);

        laya::window window("Window & Events Demo", {800, 600}, laya::window_flags::resizable);

        std::cout << "Window created with ID: " << window.id() << std::endl;
        std::cout << "Initial size: " << window.get_size().width << "x" << window.get_size().height << std::endl;
        std::cout << "Initial position: (" << window.get_position().x << ", " << window.get_position().y << ")"
                  << std::endl;

        std::cout << "\nControls:" << std::endl;
        std::cout << "  ESC - Quit" << std::endl;
        std::cout << "  M - Maximize window" << std::endl;
        std::cout << "  N - Minimize window" << std::endl;
        std::cout << "  R - Restore window" << std::endl;
        std::cout << "  H - Hide window (will show again after 2 seconds)" << std::endl;
        std::cout << "  T - Change window title" << std::endl;
        std::cout << "  Arrow keys - Move window" << std::endl;
        std::cout << "  +/- - Resize window" << std::endl;
        std::cout << "\nEvent log:" << std::endl;

        bool running = true;
        int title_counter = 0;

        while (running) {
            // Poll all events using the type-safe event system
            for (const auto& event : laya::poll_events()) {
                // Use std::visit for type-safe event handling
                std::visit(
                    [&](const auto& e) {
                        using T = std::decay_t<decltype(e)>;

                        if constexpr (std::is_same_v<T, laya::quit_event>) {
                            std::cout << "Quit event received" << std::endl;
                            running = false;

                        } else if constexpr (std::is_same_v<T, laya::window_event>) {
                            std::cout << "Window event: ";
                            switch (e.event_type) {
                                case laya::window_event::type::shown:
                                    std::cout << "shown";
                                    break;
                                case laya::window_event::type::hidden:
                                    std::cout << "hidden";
                                    break;
                                case laya::window_event::type::moved:
                                    std::cout << "moved to (" << e.data1 << ", " << e.data2 << ")";
                                    break;
                                case laya::window_event::type::resized:
                                    std::cout << "resized to " << e.data1 << "x" << e.data2;
                                    break;
                                case laya::window_event::type::size_changed:
                                    std::cout << "size changed to " << e.data1 << "x" << e.data2;
                                    break;
                                case laya::window_event::type::minimized:
                                    std::cout << "minimized";
                                    break;
                                case laya::window_event::type::maximized:
                                    std::cout << "maximized";
                                    break;
                                case laya::window_event::type::restored:
                                    std::cout << "restored";
                                    break;
                                case laya::window_event::type::enter:
                                    std::cout << "mouse entered";
                                    break;
                                case laya::window_event::type::leave:
                                    std::cout << "mouse left";
                                    break;
                                case laya::window_event::type::focus_gained:
                                    std::cout << "focus gained";
                                    break;
                                case laya::window_event::type::focus_lost:
                                    std::cout << "focus lost";
                                    break;
                                case laya::window_event::type::close:
                                    std::cout << "close requested";
                                    running = false;
                                    break;
                                default:
                                    std::cout << "other (" << static_cast<int>(e.event_type) << ")";
                                    break;
                            }
                            std::cout << std::endl;

                        } else if constexpr (std::is_same_v<T, laya::key_event>) {
                            if (e.key_state == laya::key_event::state::pressed) {
                                std::cout << "Key pressed: scancode=" << e.scancode << ", keycode=" << e.keycode;
                                if (e.repeat)
                                    std::cout << " (repeat)";
                                std::cout << std::endl;

                                // Handle specific keys
                                switch (e.scancode) {
                                    case 41:  // SDL_SCANCODE_ESCAPE
                                        running = false;
                                        break;
                                    case 16:  // SDL_SCANCODE_M
                                        window.maximize();
                                        break;
                                    case 17:  // SDL_SCANCODE_N
                                        window.minimize();
                                        break;
                                    case 21:  // SDL_SCANCODE_R
                                        window.restore();
                                        break;
                                    case 11:  // SDL_SCANCODE_H
                                        window.hide();
                                        // Note: In a real app, you'd use a timer
                                        std::cout << "Window hidden, will show again..." << std::endl;
                                        std::this_thread::sleep_for(std::chrono::seconds(2));
                                        window.show();
                                        break;
                                    case 23:  // SDL_SCANCODE_T
                                        window.set_title("New Title #" + std::to_string(++title_counter));
                                        break;
                                    case 82:  // SDL_SCANCODE_UP
                                    {
                                        auto pos = window.get_position();
                                        window.set_position({pos.x, pos.y - 10});
                                    } break;
                                    case 81:  // SDL_SCANCODE_DOWN
                                    {
                                        auto pos = window.get_position();
                                        window.set_position({pos.x, pos.y + 10});
                                    } break;
                                    case 80:  // SDL_SCANCODE_LEFT
                                    {
                                        auto pos = window.get_position();
                                        window.set_position({pos.x - 10, pos.y});
                                    } break;
                                    case 79:  // SDL_SCANCODE_RIGHT
                                    {
                                        auto pos = window.get_position();
                                        window.set_position({pos.x + 10, pos.y});
                                    } break;
                                    case 46:  // SDL_SCANCODE_EQUALS (+ key)
                                    {
                                        auto sz = window.get_size();
                                        window.set_size({sz.width + 50, sz.height + 50});
                                    } break;
                                    case 45:  // SDL_SCANCODE_MINUS
                                    {
                                        auto sz = window.get_size();
                                        if (sz.width > 100 && sz.height > 100) {
                                            window.set_size({sz.width - 50, sz.height - 50});
                                        }
                                    } break;
                                }
                            }

                        } else if constexpr (std::is_same_v<T, laya::mouse_button_event>) {
                            std::cout << "Mouse button ";
                            switch (e.mouse_button) {
                                case laya::mouse_button_event::button::left:
                                    std::cout << "left";
                                    break;
                                case laya::mouse_button_event::button::right:
                                    std::cout << "right";
                                    break;
                                case laya::mouse_button_event::button::middle:
                                    std::cout << "middle";
                                    break;
                                case laya::mouse_button_event::button::x1:
                                    std::cout << "x1";
                                    break;
                                case laya::mouse_button_event::button::x2:
                                    std::cout << "x2";
                                    break;
                            }
                            std::cout << " "
                                      << (e.button_state == laya::mouse_button_event::state::pressed ? "pressed"
                                                                                                     : "released");
                            std::cout << " at (" << e.x << ", " << e.y << ")";
                            if (e.clicks > 1)
                                std::cout << " [" << static_cast<int>(e.clicks) << " clicks]";
                            std::cout << std::endl;

                        } else if constexpr (std::is_same_v<T, laya::mouse_motion_event>) {
                            // Only log every 10th motion event to avoid spam
                            static int motion_counter = 0;
                            if (++motion_counter % 10 == 0) {
                                std::cout << "Mouse motion: (" << e.x << ", " << e.y << ") rel=(" << e.xrel << ", "
                                          << e.yrel << ")" << std::endl;
                            }

                        } else if constexpr (std::is_same_v<T, laya::mouse_wheel_event>) {
                            std::cout << "Mouse wheel: x=" << e.x << ", y=" << e.y << std::endl;

                        } else if constexpr (std::is_same_v<T, laya::text_input_event>) {
                            std::cout << "Text input: \"" << e.text << "\"" << std::endl;
                        }
                    },
                    event);
            }

            // Small delay to prevent excessive CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
        }

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
