/// @file main.cpp
/// @brief Basic renderer example demonstrating primitive drawing
/// @date 2025-10-07

#include <iostream>
#include <chrono>
#include <thread>

#include <laya/laya.hpp>

constexpr int ci_iterations = 5;  // Number of iterations in CI mode

int main(int argc, char* argv[]) {
    try {
        // Check for CI mode flag
        bool ci_mode = false;
        if (argc > 1 && std::string(argv[1]) == "--ci") {
            ci_mode = true;
            std::cout << "Running in CI mode (non-interactive)\n";
        }

        // Initialize SDL
        laya::context ctx{laya::subsystem::video};

        // Create window
        laya::window win{"Laya Renderer Example", {800, 600}, laya::window_flags::shown};

        // Create renderer
        laya::renderer renderer{win, laya::renderer_flags::accelerated};

        // Set initial draw color to white
        renderer.set_draw_color(laya::colors::white);

        bool running = true;
        int iterations = 0;
        while (running) {
            // Handle events
            for (const auto& event : laya::events_range()) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    running = false;
                }
            }

            // Clear screen with black
            renderer.set_draw_color(laya::colors::black);
            renderer.clear();

            // Draw some primitives
            {
                // Draw red rectangle outline
                auto color_guard = renderer.with_color(laya::colors::red);
                renderer.draw_rect({100, 100, 200, 150});
            }

            {
                // Draw green filled rectangle
                auto color_guard = renderer.with_color(laya::colors::green);
                renderer.fill_rect({350, 100, 200, 150});
            }

            {
                // Draw blue line
                auto color_guard = renderer.with_color(laya::colors::blue);
                renderer.draw_line({50, 300}, {750, 300});
            }

            {
                // Draw yellow points
                auto color_guard = renderer.with_color(laya::colors::yellow);
                laya::point points[] = {{100, 400}, {150, 420}, {200, 400}, {250, 420}, {300, 400}};
                renderer.draw_points(points, 5);
            }

            {
                // Draw cyan connected lines
                auto color_guard = renderer.with_color(laya::colors::cyan);
                laya::point line_points[] = {{400, 350}, {450, 400}, {500, 350}, {550, 400}, {600, 350}};
                renderer.draw_lines(line_points, 5);
            }

            {
                // Draw magenta filled rectangles
                auto color_guard = renderer.with_color(laya::colors::magenta);
                laya::rect rects[] = {{100, 500, 50, 30}, {200, 500, 50, 30}, {300, 500, 50, 30}};
                renderer.fill_rects(rects, 3);
            }

            // Present the frame
            renderer.present();

            // CI mode: exit after fixed iterations
            if (ci_mode) {
                ++iterations;
                if (iterations >= ci_iterations) {
                    std::cout << "CI mode: completed " << iterations << " iterations\n";
                    running = false;
                }
            }

            // Small delay to prevent excessive CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        std::cout << "Renderer example completed successfully!\n";
        return 0;

    } catch (const laya::error& e) {
        std::cerr << "Laya error: " << e.what() << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
