/// @file render_draw_primitives.cpp
/// @brief Basic renderer example demonstrating primitive drawing

#include <iostream>
#include <thread>

#include <laya/laya.hpp>

#include "example_base.hpp"

int main(int argc, char* argv[]) {
    auto settings = laya_examples::parse_settings(argc, argv);

    try {
        laya::context ctx{laya::subsystem::video};
        laya::window win{"render_draw_primitives", {800, 600}, laya::window_flags::shown};
        laya::renderer renderer{win, laya::renderer_flags::accelerated};

        bool running = true;
        int iterations = 0;

        while (running) {
            for (const auto& event : laya::events_range()) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    running = false;
                }
            }

            renderer.set_draw_color(laya::colors::black);
            renderer.clear();

            {
                auto color_guard = renderer.with_color(laya::colors::red);
                renderer.draw_rect({100, 100, 200, 150});
            }

            {
                auto color_guard = renderer.with_color(laya::colors::green);
                renderer.fill_rect({350, 100, 200, 150});
            }

            {
                auto color_guard = renderer.with_color(laya::colors::blue);
                renderer.draw_line({50, 300}, {750, 300});
            }

            {
                auto color_guard = renderer.with_color(laya::colors::yellow);
                laya::point points[] = {{100, 400}, {150, 420}, {200, 400}, {250, 420}, {300, 400}};
                renderer.draw_points(points, 5);
            }

            {
                auto color_guard = renderer.with_color(laya::colors::cyan);
                laya::point line_points[] = {{400, 350}, {450, 400}, {500, 350}, {550, 400}, {600, 350}};
                renderer.draw_lines(line_points, 5);
            }

            {
                auto color_guard = renderer.with_color(laya::colors::magenta);
                laya::rect rects[] = {{100, 500, 50, 30}, {200, 500, 50, 30}, {300, 500, 50, 30}};
                renderer.fill_rects(rects, 3);
            }

            renderer.present();

            if (laya_examples::tick_ci(settings, iterations)) {
                running = false;
            }

            laya_examples::sleep_frame();
        }

        std::cout << "Renderer primitives demo completed successfully!\n";
    } catch (const laya::error& e) {
        if (settings.ci_mode) {
            std::cout << "Skipping render_draw_primitives in CI: " << e.what() << '\n';
            return 0;
        }
        std::cerr << "Laya error: " << e.what() << '\n';
        return 1;
    } catch (const std::exception& e) {
        if (settings.ci_mode) {
            std::cout << "Skipping render_draw_primitives in CI: " << e.what() << '\n';
            return 0;
        }
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
