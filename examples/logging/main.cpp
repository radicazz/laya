/// @file main.cpp
/// @brief Example demonstrating laya's logging system
/// @date 2025-11-18

#include <laya/laya.hpp>

#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    try {
        // Check for CI mode flag
        bool ci_mode = false;
        if (argc > 1 && std::string(argv[1]) == "--ci") {
            ci_mode = true;
        }

        // Initialize SDL
        laya::context ctx{laya::subsystem::video};

        // ====================================================================
        // Basic logging examples
        // ====================================================================

        laya::log_info("Starting logging example...");

        // Different log levels (in increasing severity)
        laya::log_trace("This is a trace message - very verbose debug info");
        laya::log_verbose("This is a verbose message - verbose debug info");
        laya::log_debug("This is a debug message");
        laya::log_info("This is an info message");
        laya::log_warn("This is a warning message");

        // Error and critical messages automatically include source location
        laya::log_error("This is an error message with file:line info");
        laya::log_critical("This is a critical error with file:line info");

        // ====================================================================
        // Format string examples
        // ====================================================================

        laya::log_info("Window size: {}x{}", 800, 600);
        laya::log_debug("Frame time: {:.2f}ms", 16.67);

        int frame_count = 42;
        double fps = 59.94;
        laya::log_info("Frame {}: {:.2f} FPS", frame_count, fps);

        // ====================================================================
        // Category-specific logging
        // ====================================================================

        laya::log(laya::log_category::video, laya::log_priority::info, "Video subsystem initialized");
        laya::log(laya::log_category::audio, laya::log_priority::debug, "Audio buffer size: {}", 2048);
        laya::log(laya::log_category::render, laya::log_priority::verbose, "Renderer created successfully");

        // ====================================================================
        // Priority management
        // ====================================================================

        // Temporarily enable verbose logging for a specific category
        {
            laya::log_info("--- Enabling verbose logging for render category ---");
            auto guard = laya::with_log_priority(laya::log_category::render, laya::log_priority::verbose);

            laya::log(laya::log_category::render, laya::log_priority::verbose, "This verbose message will be shown");
            laya::log(laya::log_category::render, laya::log_priority::debug, "Debug render info");
        }
        // Priority automatically restored here

        laya::log_info("--- Verbose logging disabled again ---");

        // ====================================================================
        // Colored output (optional feature)
        // ====================================================================

        if (!ci_mode) {
            laya::log_info("--- Trying to enable colored console output ---");
            if (laya::enable_log_colors()) {
                laya::log_info("Colors enabled! Messages should now be colored by severity");
                laya::log_trace("Trace: dim white");
                laya::log_verbose("Verbose: white");
                laya::log_debug("Debug: cyan");
                laya::log_info("Info: green");
                laya::log_warn("Warning: yellow");
                laya::log_error("Error: red");
                laya::log_critical("Critical: bold red");

                std::this_thread::sleep_for(std::chrono::seconds(2));
                laya::disable_log_colors();
                laya::log_info("Colors disabled");
            } else {
                laya::log_warn("Colored output not supported on this terminal");
            }
        }

        // ====================================================================
        // Practical examples
        // ====================================================================

        laya::log_info("--- Creating a window (practical example) ---");

        laya::window win{"Logging Example", {800, 600}, laya::window_flags::shown};
        laya::log_info("Window created: ID={}", win.id().value());

        laya::renderer renderer{win};
        laya::log(laya::log_category::render, laya::log_priority::info, "Renderer created: ID={}",
                  renderer.id().value());

        auto size = win.get_size();
        laya::log_debug("Window dimensions: {}x{}", size.width, size.height);

        // Simple render loop demonstrating logging in context
        bool running = true;
        int iterations = 0;
        const int max_iterations = ci_mode ? 5 : 60;

        laya::log_info("Starting render loop (max {} iterations)", max_iterations);

        while (running && iterations < max_iterations) {
            for (const auto& event : laya::events_range()) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    laya::log_info("Quit event received");
                    running = false;
                    break;
                }
            }

            renderer.set_draw_color(laya::colors::black);
            renderer.clear();

            // Periodically log frame info
            if (iterations % 30 == 0) {
                laya::log_debug("Frame {}: rendering...", iterations);
            }

            renderer.present();
            iterations++;

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        laya::log_info("Render loop completed after {} iterations", iterations);
        laya::log_info("Logging example completed successfully!");

        return 0;

    } catch (const laya::error& e) {
        laya::log_critical("Laya error: {}", e.what());
        return 1;
    } catch (const std::exception& e) {
        laya::log_critical("Standard exception: {}", e.what());
        return 1;
    }
}
