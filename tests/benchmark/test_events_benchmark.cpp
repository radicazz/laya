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

constexpr int runs_per_test = 5;
constexpr int iterations = 5'000;
constexpr int events_per_iteration = 1'000;

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
    TEST_CASE("laya event_range polling benchmark") {
        laya::context ctx(laya::subsystem::video);
        laya::window window("Benchmark Window", {800, 600});

        std::vector<double> run_times;
        run_times.reserve(runs_per_test);

        for (int run = 0; run < runs_per_test; ++run) {
            double total_time = 0.0;

            for (int i = 0; i < iterations; ++i) {
                // Generate synthetic events
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
            MESSAGE("  Run ", run + 1, ": ", laya_bench::format_time(avg));
        }

        double overall_avg = std::accumulate(run_times.begin(), run_times.end(), 0.0) / runs_per_test;
        MESSAGE("laya::event_range overall average (", events_per_iteration,
                " events): ", laya_bench::format_time(overall_avg));
    }

    TEST_CASE("laya event_view polling benchmark") {
        laya::context ctx(laya::subsystem::video);
        laya::window window("Benchmark Window", {800, 600});

        std::vector<double> run_times;
        run_times.reserve(runs_per_test);

        for (int run = 0; run < runs_per_test; ++run) {
            double total_time = 0.0;

            for (int i = 0; i < iterations; ++i) {
                // Generate synthetic events
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
            MESSAGE("  Run ", run + 1, ": ", laya_bench::format_time(avg));
        }

        double overall_avg = std::accumulate(run_times.begin(), run_times.end(), 0.0) / runs_per_test;
        MESSAGE("laya::event_view overall average (", events_per_iteration,
                " events): ", laya_bench::format_time(overall_avg));
    }

    TEST_CASE("raw SDL3 event polling benchmark") {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Window* window = SDL_CreateWindow("Benchmark Window", 800, 600, 0);
        std::uint32_t window_id = SDL_GetWindowID(window);

        std::vector<double> run_times;
        run_times.reserve(runs_per_test);

        for (int run = 0; run < runs_per_test; ++run) {
            double total_time = 0.0;

            for (int i = 0; i < iterations; ++i) {
                // Generate synthetic events
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
            MESSAGE("  Run ", run + 1, ": ", laya_bench::format_time(avg));
        }

        double overall_avg = std::accumulate(run_times.begin(), run_times.end(), 0.0) / runs_per_test;
        MESSAGE("raw SDL3 overall average (", events_per_iteration, " events): ", laya_bench::format_time(overall_avg));

        SDL_DestroyWindow(window);
        SDL_Quit();
    }

}  // TEST_SUITE("benchmark")
