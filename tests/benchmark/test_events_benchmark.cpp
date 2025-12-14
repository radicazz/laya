/// @file test_events_benchmark.cpp
/// @brief Benchmark tests for event polling mechanisms
/// @date 2025-10-02

#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

#include <doctest/doctest.h>
#include <SDL3/SDL.h>
#include <laya/laya.hpp>

#include "bench_utils.hpp"

namespace {

constexpr int runs_per_test = 10;
constexpr int iterations = 500;
constexpr int events_per_iteration = 100;

// Global storage for benchmark results to enable summary printing
struct benchmark_results {
    laya_bench::statistics event_range_stats;
    laya_bench::statistics event_view_stats;
    laya_bench::statistics raw_sdl_stats;
    bool populated = false;
} g_event_benchmark_results;

/// Generate synthetic events for benchmarking
/// @param count Number of events to generate
/// @param window_id Window ID to associate events with
void generate_synthetic_events(int count, std::uint32_t window_id) {
    for (int i = 0; i < count; ++i) {
        SDL_Event event;

        // Alternate between different event types for realism
        switch (i % 4) {
            case 0: {  // Key press
                event.type = SDL_EVENT_KEY_DOWN;
                event.key.timestamp = SDL_GetTicks();
                event.key.windowID = window_id;
                event.key.scancode = SDL_SCANCODE_A;
                event.key.key = SDLK_A;
                event.key.mod = SDL_KMOD_NONE;
                event.key.repeat = false;
                break;
            }
            case 1: {  // Mouse motion
                event.type = SDL_EVENT_MOUSE_MOTION;
                event.motion.timestamp = SDL_GetTicks();
                event.motion.windowID = window_id;
                event.motion.which = 0;
                event.motion.state = 0;
                event.motion.x = static_cast<float>(i % 800);
                event.motion.y = static_cast<float>(i % 600);
                event.motion.xrel = 1.0f;
                event.motion.yrel = 1.0f;
                break;
            }
            case 2: {  // Mouse button
                event.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                event.button.timestamp = SDL_GetTicks();
                event.button.windowID = window_id;
                event.button.which = 0;
                event.button.button = SDL_BUTTON_LEFT;
                event.button.down = true;
                event.button.clicks = 1;
                event.button.x = static_cast<float>(i % 800);
                event.button.y = static_cast<float>(i % 600);
                break;
            }
            case 3: {  // Window event
                event.type = SDL_EVENT_WINDOW_EXPOSED;
                event.window.timestamp = SDL_GetTicks();
                event.window.windowID = window_id;
                event.window.data1 = 0;
                event.window.data2 = 0;
                break;
            }
        }

        SDL_PushEvent(&event);
    }
}

/// Consume all events in the queue (cleanup)
void flush_all_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Discard
    }
}

}  // anonymous namespace

TEST_SUITE("benchmark") {
    TEST_CASE("event polling performance comparison") {
        laya_bench::print_header("Event Polling Performance Comparison");

        std::cout << "\n  Configuration:\n";
        std::cout << "    Runs per test:        " << runs_per_test << "\n";
        std::cout << "    Iterations per run:   " << iterations << "\n";
        std::cout << "    Events per iteration: " << events_per_iteration << "\n";
        std::cout << "    Total events/run:     " << (iterations * events_per_iteration) << "\n";

        // Storage for all benchmark results
        laya_bench::statistics event_range_stats;
        laya_bench::statistics event_view_stats;
        laya_bench::statistics raw_sdl_stats;

        // Benchmark 1: laya::event_range
        {
            laya_bench::print_separator();
            std::cout << "\n  Running: laya::event_range benchmark...\n";

            laya::context ctx(laya::subsystem::video);
            laya::window window("Benchmark Window", {800, 600});

            std::vector<double> run_times;
            run_times.reserve(runs_per_test);

            for (int run = 0; run < runs_per_test; ++run) {
                double total_time = 0.0;

                for (int i = 0; i < iterations; ++i) {
                    generate_synthetic_events(events_per_iteration, window.id().value());

                    auto start = std::chrono::high_resolution_clock::now();

                    // Poll events using event_range
                    auto events = laya::events_range();

                    // Process events (simulate real work)
                    std::size_t count = 0;
                    for (const auto& event : events) {
                        ++count;
                        // Touch the event data to prevent optimization
                        std::visit([](const auto&) {}, event);
                    }

                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration<double, std::micro>(end - start);
                    total_time += duration.count();

                    flush_all_events();  // Ensure clean slate
                }

                double avg = total_time / iterations;
                run_times.push_back(avg);
            }

            event_range_stats = laya_bench::calculate_statistics(run_times);
            laya_bench::print_statistics("laya::event_range", event_range_stats, events_per_iteration);
        }

        // Benchmark 2: laya::event_view
        {
            laya_bench::print_separator();
            std::cout << "\n  Running: laya::event_view benchmark...\n";

            laya::context ctx(laya::subsystem::video);
            laya::window window("Benchmark Window", {800, 600});

            std::vector<double> run_times;
            run_times.reserve(runs_per_test);

            for (int run = 0; run < runs_per_test; ++run) {
                double total_time = 0.0;

                for (int i = 0; i < iterations; ++i) {
                    generate_synthetic_events(events_per_iteration, window.id().value());

                    auto start = std::chrono::high_resolution_clock::now();

                    // Poll events using event_view (lazy, zero-allocation)
                    std::size_t count = 0;
                    for (const auto& event : laya::events_view()) {
                        ++count;
                        // Touch the event data to prevent optimization
                        std::visit([](const auto&) {}, event);
                    }

                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration<double, std::micro>(end - start);
                    total_time += duration.count();

                    flush_all_events();  // Ensure clean slate
                }

                double avg = total_time / iterations;
                run_times.push_back(avg);
            }

            event_view_stats = laya_bench::calculate_statistics(run_times);
            laya_bench::print_statistics("laya::event_view", event_view_stats, events_per_iteration);
        }

        // Benchmark 3: raw SDL3
        {
            laya_bench::print_separator();
            std::cout << "\n  Running: raw SDL3 benchmark...\n";

            SDL_Init(SDL_INIT_VIDEO);
            SDL_Window* window = SDL_CreateWindow("Benchmark Window", 800, 600, 0);
            std::uint32_t window_id = SDL_GetWindowID(window);

            std::vector<double> run_times;
            run_times.reserve(runs_per_test);

            for (int run = 0; run < runs_per_test; ++run) {
                double total_time = 0.0;

                for (int i = 0; i < iterations; ++i) {
                    generate_synthetic_events(events_per_iteration, window_id);

                    auto start = std::chrono::high_resolution_clock::now();

                    // Poll events using raw SDL
                    SDL_Event event;
                    std::size_t count = 0;
                    while (SDL_PollEvent(&event)) {
                        ++count;
                        // Touch the event data to prevent optimization
                        volatile auto timestamp = event.common.timestamp;
                        (void)timestamp;
                    }

                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration<double, std::micro>(end - start);
                    total_time += duration.count();

                    flush_all_events();  // Ensure clean slate
                }

                double avg = total_time / iterations;
                run_times.push_back(avg);
            }

            raw_sdl_stats = laya_bench::calculate_statistics(run_times);
            laya_bench::print_statistics("raw SDL3", raw_sdl_stats, events_per_iteration);

            SDL_DestroyWindow(window);
            SDL_Quit();
        }

        // Comparative analysis
        laya_bench::print_separator();
        std::cout << "\n  Performance Comparisons:\n";
        laya_bench::print_comparison("raw SDL3", raw_sdl_stats, "laya::event_range", event_range_stats);
        laya_bench::print_comparison("raw SDL3", raw_sdl_stats, "laya::event_view", event_view_stats);
        laya_bench::print_comparison("laya::event_range", event_range_stats, "laya::event_view", event_view_stats);

        laya_bench::print_separator();
        std::cout << "\n";

        // Store results for summary printing
        g_event_benchmark_results.event_range_stats = event_range_stats;
        g_event_benchmark_results.event_view_stats = event_view_stats;
        g_event_benchmark_results.raw_sdl_stats = raw_sdl_stats;
        g_event_benchmark_results.populated = true;
    }

    // Summary test case - runs last due to '~' prefix (comes after all alphanumeric in ASCII)
    TEST_CASE("~benchmark_summary") {
        if (!g_event_benchmark_results.populated) {
            std::cout << "\n  Warning: Event benchmark results not available for summary\n";
            return;
        }

        // Print Laya vs SDL3 summary
        laya_bench::print_header("Laya vs SDL3 Summary");
        std::cout << "\n  Event Polling Performance:\n\n";
        std::cout << std::format("    {:20} {:>15} {:>20}\n", "Approach", "Performance", "vs SDL3");
        std::cout << "    " << std::string(60, '-') << "\n";
        std::cout << std::format("    {:20} {:>15} {:>20}\n", "raw SDL3",
                                 laya_bench::format_time_auto(g_event_benchmark_results.raw_sdl_stats.mean),
                                 "Baseline");
        std::cout << std::format(
            "    {:20} {:>15} {:>20}\n", "laya::event_view",
            laya_bench::format_time_auto(g_event_benchmark_results.event_view_stats.mean),
            std::format(
                "{:.0f}% slower ({:.2f}x)",
                ((g_event_benchmark_results.event_view_stats.mean - g_event_benchmark_results.raw_sdl_stats.mean) /
                 g_event_benchmark_results.raw_sdl_stats.mean) *
                    100.0,
                g_event_benchmark_results.event_view_stats.mean / g_event_benchmark_results.raw_sdl_stats.mean));
        std::cout << std::format(
            "    {:20} {:>15} {:>20}\n", "laya::event_range",
            laya_bench::format_time_auto(g_event_benchmark_results.event_range_stats.mean),
            std::format(
                "{:.0f}% slower ({:.2f}x)",
                ((g_event_benchmark_results.event_range_stats.mean - g_event_benchmark_results.raw_sdl_stats.mean) /
                 g_event_benchmark_results.raw_sdl_stats.mean) *
                    100.0,
                g_event_benchmark_results.event_range_stats.mean / g_event_benchmark_results.raw_sdl_stats.mean));

        std::cout << "\n  Key Insights:\n";
        std::cout << "    • event_view is the recommended choice (zero-allocation lazy iteration)\n";
        std::cout << "    • Overhead is ~"
                  << std::format("{:.2f}", g_event_benchmark_results.event_view_stats.mean -
                                               g_event_benchmark_results.raw_sdl_stats.mean)
                  << "µs per iteration for type-safe abstractions\n";
        std::cout << "    • Trade-off: Type safety + modern C++ vs raw performance\n";

        laya_bench::print_separator();
        std::cout << "\n";
    }

}  // TEST_SUITE("benchmark")
