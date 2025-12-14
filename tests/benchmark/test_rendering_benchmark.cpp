/// @file test_rendering_benchmark.cpp
/// @brief Benchmark tests for rendering operations
/// @date 2025-12-14

#include <chrono>
#include <iostream>
#include <vector>

#include <doctest/doctest.h>
#include <laya/laya.hpp>

#include "bench_utils.hpp"

namespace {

constexpr int runs_per_test = 10;
constexpr int iterations = 100;

}  // anonymous namespace

TEST_SUITE("benchmark") {
    TEST_CASE("rendering operations") {
        // Single context/window/renderer for all rendering benchmarks
        // to avoid SDL3 reinitialization issues on Linux
        laya::context ctx(laya::subsystem::video);
        laya::window window("Benchmark Window", {1920, 1080});
        laya::renderer renderer(window);

        // ====================================================================
        // Clear Operations
        // ====================================================================
        {
            laya_bench::print_header("Renderer Clear Operations");

            std::cout << "\n  Configuration:\n";
            std::cout << "    Runs per test:      " << runs_per_test << "\n";
            std::cout << "    Iterations per run: " << iterations << "\n";
            std::cout << "    Resolution:         1920x1080\n";

            laya_bench::print_separator();
            std::cout << "\n  Running: clear() benchmark...\n";

            std::vector<double> run_times;
            run_times.reserve(runs_per_test);

            for (int run = 0; run < runs_per_test; ++run) {
                double total_time = 0.0;

                for (int i = 0; i < iterations; ++i) {
                    auto start = std::chrono::high_resolution_clock::now();
                    renderer.clear();
                    auto end = std::chrono::high_resolution_clock::now();

                    auto duration = std::chrono::duration<double, std::micro>(end - start);
                    total_time += duration.count();
                }

                double avg = total_time / iterations;
                run_times.push_back(avg);
            }

            auto stats = laya_bench::calculate_statistics(run_times);
            laya_bench::print_statistics("clear()", stats, iterations);

            laya_bench::print_separator();
            std::cout << "\n";
        }

        // ====================================================================
        // Primitive Drawing Operations
        // ====================================================================
        {
            laya_bench::print_header("Primitive Drawing Operations");

            std::cout << "\n  Configuration:\n";
            std::cout << "    Runs per test:      " << runs_per_test << "\n";
            std::cout << "    Iterations per run: " << iterations << "\n";
            std::cout << "    Primitives/iter:    1000\n";

            constexpr int primitives_per_iteration = 1000;
            laya_bench::statistics point_stats, line_stats, rect_stats, filled_rect_stats;

            // Benchmark: draw_point
            {
                laya_bench::print_separator();
                std::cout << "\n  Running: draw_point() benchmark...\n";

                std::vector<double> run_times;
                run_times.reserve(runs_per_test);

                for (int run = 0; run < runs_per_test; ++run) {
                    double total_time = 0.0;

                    for (int i = 0; i < iterations; ++i) {
                        renderer.clear();

                        auto start = std::chrono::high_resolution_clock::now();
                        for (int p = 0; p < primitives_per_iteration; ++p) {
                            renderer.draw_point({p % 1920, (p * 7) % 1080});
                        }
                        auto end = std::chrono::high_resolution_clock::now();

                        auto duration = std::chrono::duration<double, std::micro>(end - start);
                        total_time += duration.count();
                    }

                    double avg = total_time / iterations;
                    run_times.push_back(avg);
                }

                point_stats = laya_bench::calculate_statistics(run_times);
                laya_bench::print_statistics("draw_point()", point_stats, primitives_per_iteration);
            }

            // Benchmark: draw_line
            {
                laya_bench::print_separator();
                std::cout << "\n  Running: draw_line() benchmark...\n";

                std::vector<double> run_times;
                run_times.reserve(runs_per_test);

                for (int run = 0; run < runs_per_test; ++run) {
                    double total_time = 0.0;

                    for (int i = 0; i < iterations; ++i) {
                        renderer.clear();

                        auto start = std::chrono::high_resolution_clock::now();
                        for (int l = 0; l < primitives_per_iteration; ++l) {
                            renderer.draw_line({l % 1920, (l * 7) % 1080}, {(l + 100) % 1920, (l * 13) % 1080});
                        }
                        auto end = std::chrono::high_resolution_clock::now();

                        auto duration = std::chrono::duration<double, std::micro>(end - start);
                        total_time += duration.count();
                    }

                    double avg = total_time / iterations;
                    run_times.push_back(avg);
                }

                line_stats = laya_bench::calculate_statistics(run_times);
                laya_bench::print_statistics("draw_line()", line_stats, primitives_per_iteration);
            }

            // Benchmark: draw_rect
            {
                laya_bench::print_separator();
                std::cout << "\n  Running: draw_rect() benchmark...\n";

                std::vector<double> run_times;
                run_times.reserve(runs_per_test);

                for (int run = 0; run < runs_per_test; ++run) {
                    double total_time = 0.0;

                    for (int i = 0; i < iterations; ++i) {
                        renderer.clear();

                        auto start = std::chrono::high_resolution_clock::now();
                        for (int r = 0; r < primitives_per_iteration; ++r) {
                            renderer.draw_rect({r % 1920, (r * 7) % 1080, 50, 50});
                        }
                        auto end = std::chrono::high_resolution_clock::now();

                        auto duration = std::chrono::duration<double, std::micro>(end - start);
                        total_time += duration.count();
                    }

                    double avg = total_time / iterations;
                    run_times.push_back(avg);
                }

                rect_stats = laya_bench::calculate_statistics(run_times);
                laya_bench::print_statistics("draw_rect()", rect_stats, primitives_per_iteration);
            }

            // Benchmark: fill_rect
            {
                laya_bench::print_separator();
                std::cout << "\n  Running: fill_rect() benchmark...\n";

                std::vector<double> run_times;
                run_times.reserve(runs_per_test);

                for (int run = 0; run < runs_per_test; ++run) {
                    double total_time = 0.0;

                    for (int i = 0; i < iterations; ++i) {
                        renderer.clear();

                        auto start = std::chrono::high_resolution_clock::now();
                        for (int r = 0; r < primitives_per_iteration; ++r) {
                            renderer.fill_rect({r % 1920, (r * 7) % 1080, 50, 50});
                        }
                        auto end = std::chrono::high_resolution_clock::now();

                        auto duration = std::chrono::duration<double, std::micro>(end - start);
                        total_time += duration.count();
                    }

                    double avg = total_time / iterations;
                    run_times.push_back(avg);
                }

                filled_rect_stats = laya_bench::calculate_statistics(run_times);
                laya_bench::print_statistics("fill_rect()", filled_rect_stats, primitives_per_iteration);
            }

            // Comparative analysis
            laya_bench::print_separator();
            std::cout << "\n  Performance Comparisons:\n";
            laya_bench::print_comparison("draw_point()", point_stats, "draw_line()", line_stats);
            laya_bench::print_comparison("draw_rect()", rect_stats, "fill_rect()", filled_rect_stats);

            laya_bench::print_separator();
            std::cout << "\n";
        }

        // ====================================================================
        // Batch vs Individual Drawing
        // ====================================================================
        {
            laya_bench::print_header("Batch vs Individual Drawing");

            std::cout << "\n  Configuration:\n";
            std::cout << "    Runs per test:      " << runs_per_test << "\n";
            std::cout << "    Iterations per run: " << iterations << "\n";
            std::cout << "    Primitives/iter:    1000\n";

            constexpr int primitives_per_iteration = 1000;
            laya_bench::statistics individual_stats, batch_stats;

            // Prepare batch data
            std::vector<laya::point> points;
            points.reserve(primitives_per_iteration);
            for (int i = 0; i < primitives_per_iteration; ++i) {
                points.push_back({i % 1920, (i * 7) % 1080});
            }

            // Benchmark: individual draw_point calls
            {
                laya_bench::print_separator();
                std::cout << "\n  Running: individual draw_point() calls...\n";

                std::vector<double> run_times;
                run_times.reserve(runs_per_test);

                for (int run = 0; run < runs_per_test; ++run) {
                    double total_time = 0.0;

                    for (int i = 0; i < iterations; ++i) {
                        renderer.clear();

                        auto start = std::chrono::high_resolution_clock::now();
                        for (const auto& pt : points) {
                            renderer.draw_point(pt);
                        }
                        auto end = std::chrono::high_resolution_clock::now();

                        auto duration = std::chrono::duration<double, std::micro>(end - start);
                        total_time += duration.count();
                    }

                    double avg = total_time / iterations;
                    run_times.push_back(avg);
                }

                individual_stats = laya_bench::calculate_statistics(run_times);
                laya_bench::print_statistics("Individual calls", individual_stats, primitives_per_iteration);
            }

            // Benchmark: batch draw_points call
            {
                laya_bench::print_separator();
                std::cout << "\n  Running: batch draw_points() call...\n";

                std::vector<double> run_times;
                run_times.reserve(runs_per_test);

                for (int run = 0; run < runs_per_test; ++run) {
                    double total_time = 0.0;

                    for (int i = 0; i < iterations; ++i) {
                        renderer.clear();

                        auto start = std::chrono::high_resolution_clock::now();
                        renderer.draw_points(points.data(), static_cast<int>(points.size()));
                        auto end = std::chrono::high_resolution_clock::now();

                        auto duration = std::chrono::duration<double, std::micro>(end - start);
                        total_time += duration.count();
                    }

                    double avg = total_time / iterations;
                    run_times.push_back(avg);
                }

                batch_stats = laya_bench::calculate_statistics(run_times);
                laya_bench::print_statistics("Batch call", batch_stats, primitives_per_iteration);
            }

            // Comparative analysis
            laya_bench::print_separator();
            std::cout << "\n  Performance Comparison:\n";
            laya_bench::print_comparison("Individual calls", individual_stats, "Batch call", batch_stats);

            laya_bench::print_separator();
            std::cout << "\n";
        }

        // ====================================================================
        // Renderer State Changes
        // ====================================================================
        {
            laya_bench::print_header("Renderer State Changes");

            std::cout << "\n  Configuration:\n";
            std::cout << "    Runs per test:      " << runs_per_test << "\n";
            std::cout << "    Iterations per run: " << iterations << "\n";
            std::cout << "    State changes/iter: 100\n";

            constexpr int state_changes = 100;
            laya_bench::statistics color_stats, blend_stats, viewport_stats;

            // Benchmark: set_draw_color
            {
                laya_bench::print_separator();
                std::cout << "\n  Running: set_draw_color() benchmark...\n";

                std::vector<double> run_times;
                run_times.reserve(runs_per_test);

                for (int run = 0; run < runs_per_test; ++run) {
                    double total_time = 0.0;

                    for (int i = 0; i < iterations; ++i) {
                        auto start = std::chrono::high_resolution_clock::now();
                        for (int c = 0; c < state_changes; ++c) {
                            renderer.set_draw_color({static_cast<std::uint8_t>(c), 128, 255, 255});
                        }
                        auto end = std::chrono::high_resolution_clock::now();

                        auto duration = std::chrono::duration<double, std::micro>(end - start);
                        total_time += duration.count();
                    }

                    double avg = total_time / iterations;
                    run_times.push_back(avg);
                }

                color_stats = laya_bench::calculate_statistics(run_times);
                laya_bench::print_statistics("set_draw_color()", color_stats, state_changes);
            }

            // Benchmark: set_blend_mode
            {
                laya_bench::print_separator();
                std::cout << "\n  Running: set_blend_mode() benchmark...\n";

                std::vector<double> run_times;
                run_times.reserve(runs_per_test);

                for (int run = 0; run < runs_per_test; ++run) {
                    double total_time = 0.0;

                    for (int i = 0; i < iterations; ++i) {
                        auto start = std::chrono::high_resolution_clock::now();
                        for (int b = 0; b < state_changes; ++b) {
                            renderer.set_blend_mode(b % 2 == 0 ? laya::blend_mode::blend : laya::blend_mode::none);
                        }
                        auto end = std::chrono::high_resolution_clock::now();

                        auto duration = std::chrono::duration<double, std::micro>(end - start);
                        total_time += duration.count();
                    }

                    double avg = total_time / iterations;
                    run_times.push_back(avg);
                }

                blend_stats = laya_bench::calculate_statistics(run_times);
                laya_bench::print_statistics("set_blend_mode()", blend_stats, state_changes);
            }

            // Benchmark: set_viewport
            {
                laya_bench::print_separator();
                std::cout << "\n  Running: set_viewport() benchmark...\n";

                std::vector<double> run_times;
                run_times.reserve(runs_per_test);

                for (int run = 0; run < runs_per_test; ++run) {
                    double total_time = 0.0;

                    for (int i = 0; i < iterations; ++i) {
                        auto start = std::chrono::high_resolution_clock::now();
                        for (int v = 0; v < state_changes; ++v) {
                            renderer.set_viewport({v % 100, v % 100, 800, 600});
                        }
                        auto end = std::chrono::high_resolution_clock::now();

                        auto duration = std::chrono::duration<double, std::micro>(end - start);
                        total_time += duration.count();
                    }

                    double avg = total_time / iterations;
                    run_times.push_back(avg);
                }

                viewport_stats = laya_bench::calculate_statistics(run_times);
                laya_bench::print_statistics("set_viewport()", viewport_stats, state_changes);
            }

            // Comparative analysis
            laya_bench::print_separator();
            std::cout << "\n  Performance Comparisons:\n";
            laya_bench::print_comparison("set_draw_color()", color_stats, "set_blend_mode()", blend_stats);
            laya_bench::print_comparison("set_draw_color()", color_stats, "set_viewport()", viewport_stats);

            laya_bench::print_separator();
            std::cout << "\n";
        }
    }

}  // TEST_SUITE("benchmark")
