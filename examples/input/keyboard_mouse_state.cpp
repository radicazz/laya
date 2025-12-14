/// Example demonstrating keyboard and mouse state queries
/// Press WASD to move, ESC to exit, mouse buttons to draw

#include <algorithm>
#include <exception>

#include <laya/laya.hpp>

#include "example_base.hpp"

int main(int argc, char* argv[]) {
    auto settings = laya_examples::parse_settings(argc, argv);

    try {
        // Initialize SDL with video subsystem
        laya::context ctx(laya::subsystem::video);

        // Create window and renderer
        laya::window win("Keyboard & Mouse State Demo", {800, 600});
        laya::renderer renderer(win);

        // Player position
        laya::point player_pos{400, 300};
        const int move_speed = 5;

        laya::log_info("Keyboard & Mouse State Demo");
        laya::log_info("Controls:");
        laya::log_info("  WASD - Move square");
        laya::log_info("  Arrow keys - Also move square");
        laya::log_info("  Shift - Move faster");
        laya::log_info("  Left click - Draw red circle");
        laya::log_info("  Right click - Draw blue circle");
        laya::log_info("  ESC or Q - Quit");

        bool running = true;
        int iterations = 0;
        while (running) {
            // Process events
            for (const auto& event : laya::events_view()) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    running = false;
                }
            }

            // Check keyboard state for movement
            int dx = 0;
            int dy = 0;

            // WASD movement
            if (laya::is_key_pressed(laya::scancode::w)) {
                dy -= move_speed;
            }
            if (laya::is_key_pressed(laya::scancode::s)) {
                dy += move_speed;
            }
            if (laya::is_key_pressed(laya::scancode::a)) {
                dx -= move_speed;
            }
            if (laya::is_key_pressed(laya::scancode::d)) {
                dx += move_speed;
            }

            // Arrow key movement
            if (laya::is_key_pressed(laya::scancode::up)) {
                dy -= move_speed;
            }
            if (laya::is_key_pressed(laya::scancode::down)) {
                dy += move_speed;
            }
            if (laya::is_key_pressed(laya::scancode::left)) {
                dx -= move_speed;
            }
            if (laya::is_key_pressed(laya::scancode::right)) {
                dx += move_speed;
            }

            // Check for Shift modifier to move faster
            if (laya::has_modifier(laya::key_modifier::shift)) {
                dx *= 2;
                dy *= 2;
            }

            // Update player position
            player_pos.x += dx;
            player_pos.y += dy;

            // Clamp to window bounds
            player_pos.x = std::max(25, std::min(775, player_pos.x));
            player_pos.y = std::max(25, std::min(575, player_pos.y));

            // Check for quit keys
            if (laya::is_key_pressed(laya::scancode::escape) || laya::is_key_pressed(laya::scancode::q)) {
                running = false;
            }

            // Get mouse state
            auto mouse_pos = laya::get_mouse_position();

            // Clear screen
            renderer.set_draw_color(laya::colors::black);
            renderer.clear();

            // Draw mouse position indicator
            renderer.set_draw_color(laya::colors::white);
            renderer.draw_line({mouse_pos.x - 10, mouse_pos.y}, {mouse_pos.x + 10, mouse_pos.y});
            renderer.draw_line({mouse_pos.x, mouse_pos.y - 10}, {mouse_pos.x, mouse_pos.y + 10});

            // Draw circles if mouse buttons are pressed
            if (laya::is_mouse_button_pressed(laya::mouse_button::left)) {
                renderer.set_draw_color(laya::colors::red);
                // Draw a simple circle approximation using rectangles
                for (int i = -5; i <= 5; ++i) {
                    for (int j = -5; j <= 5; ++j) {
                        if (i * i + j * j <= 25) {
                            renderer.fill_rect({mouse_pos.x + i, mouse_pos.y + j, 1, 1});
                        }
                    }
                }
            }

            if (laya::is_mouse_button_pressed(laya::mouse_button::right)) {
                renderer.set_draw_color(laya::colors::blue);
                // Draw a simple circle approximation using rectangles
                for (int i = -8; i <= 8; ++i) {
                    for (int j = -8; j <= 8; ++j) {
                        if (i * i + j * j <= 64) {
                            renderer.fill_rect({mouse_pos.x + i, mouse_pos.y + j, 1, 1});
                        }
                    }
                }
            }

            // Draw player square
            renderer.set_draw_color(laya::colors::green);
            renderer.fill_rect({player_pos.x - 25, player_pos.y - 25, 50, 50});

            // Draw direction indicator
            renderer.set_draw_color(laya::colors::yellow);
            if (dx != 0 || dy != 0) {
                renderer.draw_line(player_pos, {player_pos.x + dx * 3, player_pos.y + dy * 3});
            }

            renderer.present();

            if (laya_examples::tick_ci(settings, iterations)) {
                running = false;
            }

            laya_examples::sleep_frame();
        }

        laya::log_info("Exiting...");

    } catch (const laya::error& e) {
        if (settings.ci_mode) {
            std::cout << "Skipping keyboard_mouse_state in CI: " << e.what() << "\n";
            return 0;
        }
        laya::log_error("Error: {}", e.what());
        return 1;
    } catch (const std::exception& e) {
        if (settings.ci_mode) {
            std::cout << "Skipping keyboard_mouse_state in CI: " << e.what() << "\n";
            return 0;
        }
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
