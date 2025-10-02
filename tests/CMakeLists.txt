# doctest testing framework integration

# Option to use system doctest or bundled version
option(LAYA_USE_SYSTEM_DOCTEST "Use system-installed doctest" OFF)

if(LAYA_USE_SYSTEM_DOCTEST)
    find_package(doctest REQUIRED)
else()
    # Use bundled doctest submodule
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../external/doctest/doctest/doctest.h")
        message(FATAL_ERROR "doctest submodule not found. Run: git submodule update --init --recursive")
    endif()

    # Create interface library for doctest
    add_library(doctest INTERFACE)
    target_include_directories(doctest INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/../external/doctest"
    )
    add_library(doctest::doctest ALIAS doctest)
endif()

# Test configuration
set(LAYA_TEST_SOURCES
    test_main.cpp
    unit/test_subsystems.cpp
    performance/test_subsystem_performance.cpp
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

# Copy SDL shared libraries to test executable directory for runtime
laya_copy_sdl_shared_libs(laya_tests)

# Register tests with CTest
# Try to use doctest's CMake integration if available
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../external/doctest/scripts/cmake/doctest.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/../external/doctest/scripts/cmake/doctest.cmake")
    doctest_discover_tests(laya_tests)
else()
    # Manual test registration
    add_test(NAME laya_all_tests COMMAND laya_tests)

    # Add individual test suites
    add_test(NAME laya_unit_tests COMMAND laya_tests --test-suite=unit)
endif()

# Performance tests (separate executable for benchmarking)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(LAYA_BENCHMARK_SOURCES
        utils/test_main.cpp
        performance/test_subsystem_performance.cpp
    )

    add_executable(laya_benchmarks ${LAYA_BENCHMARK_SOURCES})

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

    # Copy SDL shared libraries to benchmark executable directory for runtime
    laya_copy_sdl_shared_libs(laya_benchmarks)

    add_test(NAME laya_benchmarks COMMAND laya_benchmarks --test-suite=performance)
endif()
