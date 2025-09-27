#include <doctest/doctest.h>
#include <laya/init.hpp>
#include <chrono>
#include <vector>

TEST_SUITE("performance") {
    TEST_CASE("init/quit performance benchmark") {
        constexpr int iterations = 10;  // Reduced for practical testing
        std::vector<double> times;
        times.reserve(iterations);

        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();

            laya::init();
            laya::quit();

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(end - start);
            times.push_back(duration.count());
        }

        // Calculate statistics
        double total = 0.0;
        for (double time : times) {
            total += time;
        }
        double average = total / iterations;

        // Performance expectations (reasonable for init/quit cycle)
        CHECK(average < 100.0);  // Average should be less than 100ms

        MESSAGE("Average init/quit time: ", average, " ms");
        MESSAGE("Total iterations: ", iterations);
    }

}  // TEST_SUITE("performance")
