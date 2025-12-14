/// @file bench_utils.hpp
/// @brief Benchmark testing utilities
/// @date 2025-12-14

#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <format>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

namespace laya_bench {

/// @brief Statistical results from benchmark runs
struct statistics {
    double min;
    double max;
    double mean;
    double median;
    double stddev;
    double p95;  // 95th percentile
    double p99;  // 99th percentile
    std::size_t samples;
};

/// @brief Calculate statistics from a series of measurements
/// @param values Vector of measurement values in microseconds
/// @return Statistical analysis of the data
inline statistics calculate_statistics(std::vector<double> values) {
    if (values.empty()) {
        return {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0};
    }

    std::sort(values.begin(), values.end());

    statistics stats{};
    stats.samples = values.size();
    stats.min = values.front();
    stats.max = values.back();

    // Mean
    stats.mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();

    // Median
    if (values.size() % 2 == 0) {
        stats.median = (values[values.size() / 2 - 1] + values[values.size() / 2]) / 2.0;
    } else {
        stats.median = values[values.size() / 2];
    }

    // Standard deviation
    double variance = 0.0;
    for (double value : values) {
        variance += (value - stats.mean) * (value - stats.mean);
    }
    variance /= values.size();
    stats.stddev = std::sqrt(variance);

    // Percentiles
    auto percentile_index = [&](double p) -> std::size_t {
        return static_cast<std::size_t>((p / 100.0) * (values.size() - 1));
    };
    stats.p95 = values[percentile_index(95.0)];
    stats.p99 = values[percentile_index(99.0)];

    return stats;
}

/// @brief Format microseconds as "0.123456s / 123.456ms / 123456.789µs"
/// @param microseconds Time in microseconds
/// @return Formatted string with multiple units
inline std::string format_time(double microseconds) {
    double seconds = microseconds / 1'000'000.0;
    double milliseconds = microseconds / 1'000.0;
    return std::format("{:.6f}s / {:.3f}ms / {:.2f}µs", seconds, milliseconds, microseconds);
}

/// @brief Format time in the most appropriate unit
/// @param microseconds Time in microseconds
/// @return Formatted string with the most suitable unit
inline std::string format_time_auto(double microseconds) {
    if (microseconds < 1.0) {
        return std::format("{:.2f}ns", microseconds * 1000.0);
    } else if (microseconds < 1000.0) {
        return std::format("{:.2f}µs", microseconds);
    } else if (microseconds < 1'000'000.0) {
        return std::format("{:.3f}ms", microseconds / 1000.0);
    } else {
        return std::format("{:.6f}s", microseconds / 1'000'000.0);
    }
}

/// @brief Calculate throughput (items per second)
/// @param items Number of items processed
/// @param microseconds Time taken in microseconds
/// @return Items processed per second
inline double calculate_throughput(std::size_t items, double microseconds) {
    if (microseconds <= 0.0) {
        return 0.0;
    }
    return (static_cast<double>(items) / microseconds) * 1'000'000.0;
}

/// @brief Format throughput with appropriate suffix
/// @param items_per_second Throughput in items/second
/// @return Formatted string with K/M/G suffix
inline std::string format_throughput(double items_per_second) {
    if (items_per_second >= 1'000'000'000.0) {
        return std::format("{:.2f}G/s", items_per_second / 1'000'000'000.0);
    } else if (items_per_second >= 1'000'000.0) {
        return std::format("{:.2f}M/s", items_per_second / 1'000'000.0);
    } else if (items_per_second >= 1'000.0) {
        return std::format("{:.2f}K/s", items_per_second / 1'000.0);
    } else {
        return std::format("{:.2f}/s", items_per_second);
    }
}

/// @brief Print a formatted statistics report
/// @param label Name of the benchmark
/// @param stats Statistical results
/// @param items Number of items processed (for throughput calculation)
inline void print_statistics(const std::string& label, const statistics& stats, std::size_t items = 0) {
    std::cout << "\n  " << label << ":\n";
    std::cout << "    Mean:   " << format_time_auto(stats.mean) << "\n";
    std::cout << "    Median: " << format_time_auto(stats.median) << "\n";
    std::cout << "    Min:    " << format_time_auto(stats.min) << "\n";
    std::cout << "    Max:    " << format_time_auto(stats.max) << "\n";
    std::cout << "    StdDev: " << format_time_auto(stats.stddev)
              << std::format(" ({:.1f}%)", (stats.stddev / stats.mean) * 100.0) << "\n";
    std::cout << "    P95:    " << format_time_auto(stats.p95) << "\n";
    std::cout << "    P99:    " << format_time_auto(stats.p99) << "\n";

    if (items > 0) {
        double throughput = calculate_throughput(items, stats.mean);
        std::cout << "    Throughput: " << format_throughput(throughput) << "\n";
    }
}

/// @brief Compare two benchmark results and show percentage difference
/// @param baseline_label Name of baseline benchmark
/// @param baseline_stats Statistics of baseline
/// @param comparison_label Name of comparison benchmark
/// @param comparison_stats Statistics of comparison
inline void print_comparison(const std::string& baseline_label, const statistics& baseline_stats,
                             const std::string& comparison_label, const statistics& comparison_stats) {
    double diff_percent = ((comparison_stats.mean - baseline_stats.mean) / baseline_stats.mean) * 100.0;
    double speedup = baseline_stats.mean / comparison_stats.mean;

    std::cout << "\n  Comparison (" << comparison_label << " vs " << baseline_label << "):\n";

    if (diff_percent < 0) {
        std::cout << "    " << comparison_label << " is " << std::format("{:.1f}%", -diff_percent)
                  << " FASTER (speedup: " << std::format("{:.2f}x", speedup) << ")\n";
    } else if (diff_percent > 0) {
        std::cout << "    " << comparison_label << " is " << std::format("{:.1f}%", diff_percent)
                  << " SLOWER (slowdown: " << std::format("{:.2f}x", 1.0 / speedup) << ")\n";
    } else {
        std::cout << "    Performance is IDENTICAL\n";
    }
}

/// @brief Print a separator line
inline void print_separator() {
    std::cout << "\n  " << std::string(60, '-') << "\n";
}

/// @brief Print a benchmark header
/// @param title Title of the benchmark section
inline void print_header(const std::string& title) {
    std::cout << "\n" << std::string(64, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(64, '=') << "\n";
}

}  // namespace laya_bench
