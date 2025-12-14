# Laya Benchmark Suite

## Overview

The benchmark suite provides comprehensive performance testing for Laya's core features. Benchmarks are built in Release mode with full optimizations (-O3) to ensure accurate performance measurements.

## Running Benchmarks

```bash
./scripts/run-benchmarks.sh
```

This script will:
1. Configure CMake in Release mode
2. Build the benchmark executable
3. Run all benchmark tests
4. Display detailed performance statistics

## Available Benchmarks

### Event Polling (`test_events_benchmark.cpp`)

Compares performance of different event polling approaches:
- **laya::event_range** - Pre-allocated vector-based event polling
- **laya::event_view** - Zero-allocation lazy event iteration
- **raw SDL3** - Direct SDL3 event polling for baseline comparison

**Key Metrics:**
- Events processed per second (throughput)
- Overhead of Laya abstractions vs raw SDL3
- Performance difference between allocation strategies

### Rendering Operations (`test_rendering_benchmark.cpp`)

Comprehensive rendering performance tests:

#### 1. Clear Operations
- Measures `renderer.clear()` performance
- Tests full-screen buffer clearing at 1920x1080

#### 2. Primitive Drawing
- **draw_point()** - Individual point rendering
- **draw_line()** - Line segment rendering
- **draw_rect()** - Rectangle outline drawing
- **fill_rect()** - Filled rectangle rendering
- Compares performance across different primitive types

#### 3. Batch vs Individual Drawing
- **Individual calls** - Loop calling `draw_point()` 1000 times
- **Batch call** - Single `draw_points()` call with 1000 points
- Demonstrates performance benefits of batching

#### 4. State Changes
- **set_draw_color()** - Color switching overhead
- **set_blend_mode()** - Blend mode switching overhead
- **set_viewport()** - Viewport changes overhead
- Helps identify state change costs in render loops

## Statistical Output

Each benchmark provides comprehensive statistics:

- **Mean** - Average execution time
- **Median** - Middle value (robust to outliers)
- **Min/Max** - Best and worst case times
- **StdDev** - Standard deviation (consistency indicator)
- **P95/P99** - 95th and 99th percentile (tail latency)
- **Throughput** - Operations per second

## Performance Comparisons

Benchmarks automatically compare results:
- Percentage differences (faster/slower)
- Speedup/slowdown ratios
- Side-by-side statistical analysis

## Benchmark Configuration

Default settings (can be modified in source files):

| Parameter | Events | Rendering |
|-----------|--------|-----------|
| Runs per test | 10 | 10 |
| Iterations per run | 500 | 100 |
| Items per iteration | 100 events | 100-1,000 primitives |

**Note:** These conservative values prevent excessive memory usage while still providing statistically valid results. Each benchmark runs 10 iterations to calculate meaningful statistics (mean, median, standard deviation, percentiles).

## Performance Summary: Laya vs SDL3

The benchmarks compare Laya's abstractions against raw SDL3 to quantify overhead:

### Event Polling
| Approach | Typical Performance | vs SDL3 |
|----------|-------------------|---------|
| **raw SDL3** | ~1.15µs (87M events/s) | Baseline |
| **laya::event_view** | ~2.03µs (49M events/s) | ~75% slower (1.76x) |
| **laya::event_range** | ~2.30µs (44M events/s) | ~95% slower (2.00x) |

**Key Insight**: `event_view` is **~12% faster** than `event_range` due to zero-allocation lazy iteration, making it the preferred choice for event loops.

### Overhead Analysis
Laya's event abstractions add overhead for:
- Type-safe variant conversion (SDL_Event → std::variant)
- Iterator infrastructure
- Pre-allocation in `event_range`

**Trade-offs**: The overhead is acceptable for most applications (~1µs per iteration) in exchange for:
- Type safety and compile-time error detection
- Modern C++ range-based iteration
- Elimination of raw pointer handling
- Cleaner, more maintainable code

### Rendering Performance
All rendering operations use SDL3 directly under the hood, so performance is identical to raw SDL3.

## Interpreting Results

### Event Polling
- **raw SDL3** establishes baseline performance
- Compare Laya abstractions against baseline to measure overhead
- `event_view` should outperform `event_range` due to zero-allocation design

### Rendering
- **Batch operations** (~41x speedup) significantly outperform individual calls
- **State changes** have measurable overhead - minimize in hot loops
- **clear()** is very fast - use liberally
- **fill_rect()** is ~42% faster than **draw_rect()** due to GPU fill optimization

## Adding New Benchmarks

1. Create new file in `tests/benchmark/` (e.g., `test_texture_benchmark.cpp`)
2. Include `bench_utils.hpp` for statistical utilities
3. Use `TEST_SUITE("benchmark")` and `TEST_CASE()` from doctest
4. Add file to `LAYA_BENCHMARK_SOURCES` in `tests/CMakeLists.txt`

### Example Template

```cpp
#include "bench_utils.hpp"
#include <doctest/doctest.h>
#include <laya/laya.hpp>

TEST_SUITE("benchmark") {
    TEST_CASE("my feature benchmark") {
        laya_bench::print_header("My Feature Benchmark");
        
        std::vector<double> run_times;
        // ... perform measurements ...
        
        auto stats = laya_bench::calculate_statistics(run_times);
        laya_bench::print_statistics("Feature", stats, items_processed);
    }
}
```

## Utility Functions

### Available in `bench_utils.hpp`:

- `calculate_statistics(values)` - Compute full statistical analysis
- `format_time_auto(microseconds)` - Auto-scale time formatting
- `format_throughput(items_per_sec)` - Format with K/M/G suffixes
- `print_statistics(label, stats, items)` - Display formatted results
- `print_comparison(baseline, comparison)` - Show performance delta
- `print_header(title)` - Section header
- `print_separator()` - Visual separator line

## Build Configuration

Benchmarks use Release mode optimizations:
- **GCC/Clang**: `-O3 -DNDEBUG`
- **MSVC**: `/O2 /DNDEBUG`

A warning is issued if built in Debug mode, as results won't be representative.

## Future Enhancements

Planned benchmark additions:
- **Window operations** - Creation, resizing, state changes
- **Texture operations** - Creation, updates, rendering
- **Surface operations** - Pixel manipulation, format conversion
- **Logging** - Message throughput at different priority levels
- **Input polling** - Keyboard/mouse state queries

See `/home/slave/Coding/radicazz/laya/TODO.md` for tracking.
