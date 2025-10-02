/// @file test_subsystem_performance.cpp
/// @brief Performance benchmarks comparing laya vs raw SDL3
/// @date 2025-10-01

#include <chrono>
#include <format>
#include <iostream>

#include <doctest/doctest.h>
#include <SDL3/SDL.h>
#include <laya/laya.hpp>

#include "perf_utils.hpp"

TEST_SUITE("performance") {
    TEST_CASE("laya subsystem initialization benchmark") {
        constexpr int iterations = 100;

        // Benchmark laya initialization
        double laya_total_time = 0.0;
        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();

            laya::context ctx(laya::subsystem::video);

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::micro>(end - start);
            laya_total_time += duration.count();
        }
        double laya_avg = laya_total_time / iterations;

        MESSAGE("laya average: ", laya_perf::format_time(laya_avg));
    }

    TEST_CASE("SDL3 subsystem initialization benchmark") {
        constexpr int iterations = 100;

        // Benchmark raw SDL3 initialization
        double sdl_total_time = 0.0;
        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();

            SDL_Init(SDL_INIT_VIDEO);
            SDL_Quit();

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::micro>(end - start);
            sdl_total_time += duration.count();
        }
        double sdl_avg = sdl_total_time / iterations;

        MESSAGE("SDL3 average: ", laya_perf::format_time(sdl_avg));
    }
}  // TEST_SUITE("performance")
