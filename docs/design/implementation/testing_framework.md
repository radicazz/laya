# Testing Framework Implementation Plan

## Overview

This implementation plan covers the integration of doctest testing framework into the laya project, providing comprehensive test coverage with CI/CD pipeline compatibility.

## Goals

- Integrate doctest as the primary testing framework
- Create comprehensive test structure for unit, integration, and performance testing
- Ensure CI/CD pipeline can discover and execute tests
- Establish testing best practices and conventions
- Provide foundation for continuous testing throughout development

## Testing Strategy

### Framework Choice: doctest

**Why doctest:**

- Header-only library (no external dependencies)
- Fast compilation and execution
- Excellent CMake integration
- Modern C++ features support
- Minimal boilerplate code
- CI/CD friendly output formats

### Test Categories

1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Test component interactions and SDL3 integration
3. **Performance Tests**: Benchmark performance vs raw SDL3
4. **Regression Tests**: Prevent known issues from reoccurring

## Directory Structure

```
tests/
├── CMakeLists.txt              # Main test configuration
├── doctest/                    # doctest framework (header-only)
│   └── doctest.h              # Single header file
├── unit/                       # Unit tests
│   ├── test_init.cpp          # Test laya::init() and laya::quit()
│   ├── test_error_handling.cpp # Test error handling (future)
│   └── test_types.cpp         # Test basic types (future)
├── integration/                # Integration tests
│   ├── test_sdl_integration.cpp # Test SDL3 integration
│   └── test_raii_behavior.cpp  # Test RAII resource management
├── performance/                # Performance benchmarks
│   ├── test_init_performance.cpp # Benchmark init/quit performance
│   └── benchmark_utils.hpp     # Shared benchmark utilities
└── utils/                      # Test utilities
    ├── test_main.cpp          # Main test runner
    └── test_helpers.hpp       # Common test helpers
```

## Implementation Steps

### Step 1: Download and Setup doctest

**Duration**: 30 minutes

```bash
# Download doctest header
curl -L https://github.com/doctest/doctest/releases/latest/download/doctest.h \
     -o tests/doctest/doctest.h
```

**Alternative: Git submodule approach**

```bash
git submodule add https://github.com/doctest/doctest.git external/doctest
```

### Step 2: Configure CMake Integration

**File**: `tests/CMakeLists.txt`

```cmake
# doctest testing framework integration

# Option to use system doctest or bundled version
option(LAYA_USE_SYSTEM_DOCTEST "Use system-installed doctest" OFF)

if(LAYA_USE_SYSTEM_DOCTEST)
    find_package(doctest REQUIRED)
else()
    # Use bundled doctest header
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/doctest/doctest.h")
        message(FATAL_ERROR "doctest.h not found. Please download it to tests/doctest/")
    endif()
    
    # Create interface library for doctest
    add_library(doctest INTERFACE)
    target_include_directories(doctest INTERFACE 
        "${CMAKE_CURRENT_SOURCE_DIR}/doctest"
    )
    add_library(doctest::doctest ALIAS doctest)
endif()

# Test configuration
set(LAYA_TEST_SOURCES
    utils/test_main.cpp
    unit/test_init.cpp
    integration/test_sdl_integration.cpp
)

# Create test executable
add_executable(laya_tests ${LAYA_TEST_SOURCES})

target_link_libraries(laya_tests
    PRIVATE
        laya::laya
        doctest::doctest
)

target_compile_features(laya_tests PRIVATE cxx_std_20)

# Configure test properties
set_target_properties(laya_tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Enable compiler warnings for tests
if(MSVC)
    target_compile_options(laya_tests PRIVATE /W4)
else()
    target_compile_options(laya_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Register tests with CTest
include(doctest)
doctest_discover_tests(laya_tests)

# Alternative manual test registration if doctest_discover_tests not available
if(NOT COMMAND doctest_discover_tests)
    add_test(NAME laya_all_tests COMMAND laya_tests)
    
    # Add individual test suites
    add_test(NAME laya_unit_tests COMMAND laya_tests --test-suite=unit)
    add_test(NAME laya_integration_tests COMMAND laya_tests --test-suite=integration)
    add_test(NAME laya_performance_tests COMMAND laya_tests --test-suite=performance)
endif()

# Performance tests (separate executable for benchmarking)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(laya_benchmarks
        utils/test_main.cpp
        performance/test_init_performance.cpp
    )
    
    target_link_libraries(laya_benchmarks
        PRIVATE
            laya::laya
            doctest::doctest
    )
    
    target_compile_features(laya_benchmarks PRIVATE cxx_std_20)
    
    # Optimize benchmarks
    target_compile_options(laya_benchmarks PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -DNDEBUG>
        $<$<CXX_COMPILER_ID:MSVC>:/O2 /DNDEBUG>
    )
    
    add_test(NAME laya_benchmarks COMMAND laya_benchmarks --test-suite=benchmark)
endif()
```

### Step 3: Create Test Main Runner

**File**: `tests/utils/test_main.cpp`

```cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// Global test configuration
namespace {
    struct GlobalTestSetup {
        GlobalTestSetup() {
            // Global test setup if needed
        }
        
        ~GlobalTestSetup() {
            // Global test cleanup if needed
        }
    };
    
    GlobalTestSetup g_test_setup;
}
```

### Step 4: Create Test Helpers

**File**: `tests/utils/test_helpers.hpp`

```cpp
#pragma once

#include <doctest/doctest.h>
#include <laya/init.hpp>
#include <stdexcept>

namespace laya::test {

/// RAII helper for SDL initialization in tests
class SdlTestContext {
public:
    SdlTestContext() {
        laya::init();
    }
    
    ~SdlTestContext() noexcept {
        laya::quit();
    }
    
    // Non-copyable, non-movable
    SdlTestContext(const SdlTestContext&) = delete;
    SdlTestContext& operator=(const SdlTestContext&) = delete;
    SdlTestContext(SdlTestContext&&) = delete;
    SdlTestContext& operator=(SdlTestContext&&) = delete;
};

/// Helper macro for tests that need SDL initialization
#define LAYA_TEST_WITH_SDL(name) \
    TEST_CASE(name) { \
        laya::test::SdlTestContext ctx;

/// Helper to check if an exception is thrown
template<typename Exception, typename Callable>
bool throws_exception(Callable&& callable) {
    try {
        callable();
        return false;
    } catch (const Exception&) {
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace laya::test
```

### Step 5: Create Initial Unit Tests

**File**: `tests/unit/test_init.cpp`

```cpp
#include <doctest/doctest.h>
#include <laya/init.hpp>
#include "../utils/test_helpers.hpp"

TEST_SUITE("unit") {

TEST_CASE("init and quit basic functionality") {
    // Test that init and quit can be called without throwing
    CHECK_NOTHROW(laya::init());
    CHECK_NOTHROW(laya::quit());
}

TEST_CASE("multiple init calls are safe") {
    // SDL3 should handle multiple init calls gracefully
    CHECK_NOTHROW(laya::init());
    CHECK_NOTHROW(laya::init());  // Second call should be safe
    CHECK_NOTHROW(laya::quit());
    CHECK_NOTHROW(laya::quit());  // Multiple quits should be safe
}

TEST_CASE("init after quit works") {
    CHECK_NOTHROW(laya::init());
    CHECK_NOTHROW(laya::quit());
    CHECK_NOTHROW(laya::init());  // Should be able to reinitialize
    CHECK_NOTHROW(laya::quit());
}

TEST_CASE("quit is noexcept") {
    // Verify quit is marked noexcept and doesn't throw
    static_assert(noexcept(laya::quit()));
    
    laya::init();
    CHECK_NOTHROW(laya::quit());
}

} // TEST_SUITE("unit")
```

### Step 6: Create Integration Tests

**File**: `tests/integration/test_sdl_integration.cpp`

```cpp
#include <doctest/doctest.h>
#include <laya/init.hpp>
#include "../utils/test_helpers.hpp"
#include <SDL3/SDL.h>

TEST_SUITE("integration") {

LAYA_TEST_WITH_SDL("SDL subsystems are initialized") {
    // Test that SDL subsystems are properly initialized
    // This test verifies that laya::init() properly initializes SDL
    
    // Check that SDL is initialized (this is implementation-dependent)
    // We can test by trying to use SDL functions that require initialization
    
    // For example, getting SDL version should work after init
    int major, minor, patch;
    SDL_GetVersion(&major, &minor, &patch);
    
    CHECK(major >= 3);  // Should be SDL3
    CHECK(minor >= 0);
    CHECK(patch >= 0);
}

LAYA_TEST_WITH_SDL("SDL error state is clean after init") {
    // Verify that SDL error state is clean after successful initialization
    const char* error = SDL_GetError();
    CHECK((error == nullptr || strlen(error) == 0));
}

TEST_CASE("init failure handling") {
    // This test is tricky since we can't easily force SDL_Init to fail
    // We'll test the error handling path when we have more complex init logic
    
    // For now, just verify that if init throws, the state is consistent
    // This will be expanded when we have more sophisticated error handling
    WARN("Init failure testing requires more complex initialization logic");
}

} // TEST_SUITE("integration")
```

### Step 7: Create Performance Tests

**File**: `tests/performance/test_init_performance.cpp`

```cpp
#include <doctest/doctest.h>
#include <laya/init.hpp>
#include <chrono>
#include <vector>

TEST_SUITE("performance") {

TEST_CASE("init/quit performance benchmark") {
    constexpr int iterations = 100;
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
    
    // Performance expectations (these are rough estimates)
    CHECK(average < 50.0);  // Average should be less than 50ms
    
    MESSAGE("Average init/quit time: ", average, " ms");
    MESSAGE("Total iterations: ", iterations);
}

TEST_CASE("repeated init performance") {
    constexpr int iterations = 1000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        laya::init();
        // Don't quit between iterations to test repeated init calls
    }
    laya::quit();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    MESSAGE("Repeated init time for ", iterations, " calls: ", duration.count(), " ms");
    CHECK(duration.count() < 1000.0);  // Should complete in less than 1 second
}

} // TEST_SUITE("performance")
```

### Step 8: Create Benchmark Utilities

**File**: `tests/performance/benchmark_utils.hpp`

```cpp
#pragma once

#include <chrono>
#include <functional>
#include <vector>
#include <algorithm>
#include <numeric>

namespace laya::test::benchmark {

struct BenchmarkResult {
    double min_time_ms;
    double max_time_ms;
    double avg_time_ms;
    double median_time_ms;
    size_t iterations;
};

/// Simple benchmark runner
template<typename Callable>
BenchmarkResult run_benchmark(Callable&& func, size_t iterations = 100) {
    std::vector<double> times;
    times.reserve(iterations);
    
    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration<double, std::milli>(end - start);
        times.push_back(duration.count());
    }
    
    std::sort(times.begin(), times.end());
    
    BenchmarkResult result;
    result.min_time_ms = times.front();
    result.max_time_ms = times.back();
    result.avg_time_ms = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    result.median_time_ms = times[times.size() / 2];
    result.iterations = iterations;
    
    return result;
}

} // namespace laya::test::benchmark
```

## CI/CD Integration

### CTest Configuration

The tests are automatically discoverable by CTest through the `doctest_discover_tests()` function or manual test registration. This ensures compatibility with CI/CD pipelines.

### Test Execution Commands

```bash
# Configure and build with tests
cmake -B build -DLAYA_BUILD_TESTS=ON
cmake --build build

# Run all tests
ctest --test-dir build

# Run specific test suites
ctest --test-dir build -R "unit"
ctest --test-dir build -R "integration"
ctest --test-dir build -R "performance"

# Run tests with verbose output
ctest --test-dir build --verbose

# Run tests in parallel
ctest --test-dir build --parallel 4
```

### GitHub Actions Integration

```yaml
# Example CI configuration snippet
- name: Run Tests
  run: |
    cmake -B build -DLAYA_BUILD_TESTS=ON
    cmake --build build --config Release
    ctest --test-dir build --output-on-failure --parallel 2
```

## Testing Best Practices

### Test Naming Conventions

- Test files: `test_<component>.cpp`
- Test cases: Descriptive names in lowercase with spaces
- Test suites: Component or category names

### Test Organization

- **Unit tests**: Test single functions/classes in isolation
- **Integration tests**: Test component interactions
- **Performance tests**: Benchmark critical paths
- **Regression tests**: Prevent known bugs from returning

### Test Data Management

- Use temporary files for file I/O tests
- Clean up resources in test destructors
- Use RAII helpers for SDL initialization

### Error Testing

- Test both success and failure paths
- Verify exception types and messages
- Test resource cleanup on errors

## Implementation Timeline

### Week 1: Basic Framework Setup

- [ ] Download and integrate doctest
- [ ] Configure CMake integration
- [ ] Create test main runner and helpers
- [ ] Implement basic unit tests for init/quit

### Week 2: Comprehensive Testing

- [ ] Add integration tests for SDL interaction
- [ ] Create performance benchmarks
- [ ] Set up CTest integration
- [ ] Verify CI/CD compatibility

### Week 3: Advanced Testing Features

- [ ] Add memory leak detection
- [ ] Create test coverage reporting
- [ ] Add sanitizer integration
- [ ] Document testing procedures

## Success Criteria

- [ ] All tests compile and run successfully
- [ ] CTest discovers and executes all tests
- [ ] CI/CD pipeline can run tests automatically
- [ ] Test coverage for all public APIs
- [ ] Performance benchmarks establish baselines
- [ ] Memory leak detection shows no leaks
- [ ] Cross-platform test execution works

## Risk Mitigation

### Potential Issues

1. **doctest Integration**: Header-only nature should minimize issues
2. **SDL3 Testing**: Some SDL functions may require specific environments
3. **Performance Variance**: Benchmarks may vary across systems
4. **CI/CD Compatibility**: Test discovery must work across platforms

### Mitigation Strategies

- Use bundled doctest to avoid version conflicts
- Mock SDL functions for unit tests where needed
- Use relative performance comparisons rather than absolute times
- Test CI/CD integration early and often

## Future Enhancements

- **Code Coverage**: Integrate gcov/lcov for coverage reporting
- **Fuzzing**: Add fuzz testing for input validation
- **Property Testing**: Add property-based testing for complex scenarios
- **Visual Testing**: Add tests for rendering output validation
- **Stress Testing**: Add long-running stress tests for stability
