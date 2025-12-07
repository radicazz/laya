#include <laya/laya.hpp>

#include "example_base.hpp"

int main(int argc, char* argv[]) {
    auto settings = laya_examples::parse_settings(argc, argv);

    try {
        laya::context ctx{laya::subsystem::video};
        laya::window win{"logging_render_loop", {800, 600}, laya::window_flags::shown};
        laya::renderer renderer{win};

        bool running = true;
        int iterations = 0;
        const int max_iterations = settings.ci_mode ? settings.ci_iterations : 120;

        laya::log_info("Starting render loop (max {} iterations)", max_iterations);

        while (running && iterations < max_iterations) {
            for (const auto& event : laya::events_range()) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    laya::log_info("Quit event received");
                    running = false;
                }
            }

            renderer.set_draw_color(laya::colors::black);
            renderer.clear();

            if (iterations % 30 == 0) {
                laya::log_debug("Frame {} rendering", iterations);
            }

            renderer.present();
            ++iterations;

            laya_examples::sleep_frame();
        }

        laya::log_info("Render loop completed after {} iterations", iterations);
    } catch (const laya::error& e) {
        if (settings.ci_mode) {
            laya::log_warn("Skipping render loop in CI: {}", e.what());
            return 0;
        }
        laya::log_critical("Laya error: {}", e.what());
        return 1;
    } catch (const std::exception& e) {
        if (settings.ci_mode) {
            laya::log_warn("Skipping render loop in CI: {}", e.what());
            return 0;
        }
        laya::log_critical("Standard exception: {}", e.what());
        return 1;
    }

    return 0;
}
