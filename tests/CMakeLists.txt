# doctest testing framework integration

# Option to use system doctest or bundled version
option(LAYA_USE_SYSTEM_DOCTEST "Use system-installed doctest" OFF)

# Test build options
option(LAYA_TESTS_UNIT "Build unit tests" ON)
option(LAYA_TESTS_BENCHMARK "Build benchmark tests" ON)

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

# Unit tests
if(LAYA_TESTS_UNIT)
    set(LAYA_UNIT_TEST_SOURCES
        test_main.cpp
        unit/test_subsystems.cpp
        unit/test_event_view.cpp
        unit/test_event_range.cpp
    )

    # Create unit test executable
    add_executable(laya_unit_tests ${LAYA_UNIT_TEST_SOURCES})

    target_link_libraries(laya_unit_tests
        PRIVATE
        laya::laya
        doctest::doctest
    )

    target_compile_features(laya_unit_tests PRIVATE cxx_std_20)

    # Configure test properties
    set_target_properties(laya_unit_tests PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    # Enable compiler warnings for tests
    if(MSVC)
        target_compile_options(laya_unit_tests PRIVATE /W4)
    else()
        target_compile_options(laya_unit_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    # Copy SDL shared libraries to test executable directory for runtime
    laya_copy_sdl_shared_libs(laya_unit_tests)

    # Register tests with CTest
    # Try to use doctest's CMake integration if available
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../external/doctest/scripts/cmake/doctest.cmake")
        include("${CMAKE_CURRENT_SOURCE_DIR}/../external/doctest/scripts/cmake/doctest.cmake")
        doctest_discover_tests(laya_unit_tests)
    else()
        # Manual test registration
        add_test(NAME laya_all_unit_tests COMMAND laya_unit_tests)
        add_test(NAME laya_unit_tests COMMAND laya_unit_tests --test-suite=unit)
    endif()
endif()

# Benchmark tests
if(LAYA_TESTS_BENCHMARK)
    set(LAYA_BENCHMARK_SOURCES
        test_main.cpp
        benchmark/test_subsystem_benchmark.cpp
    )

    add_executable(laya_benchmarks ${LAYA_BENCHMARK_SOURCES})

    target_link_libraries(laya_benchmarks
        PRIVATE
        laya::laya
        doctest::doctest
    )

    target_compile_features(laya_benchmarks PRIVATE cxx_std_20)

    # Configure benchmark properties
    set_target_properties(laya_benchmarks PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    # Optimize benchmarks in Release mode
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(laya_benchmarks PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -DNDEBUG>
            $<$<CXX_COMPILER_ID:MSVC>:/O2 /DNDEBUG>
        )
    else()
        message(WARNING "It's recommended to build benchmarks in Release mode for accurate performance measurements.")
    endif()

    # Enable compiler warnings for benchmarks
    if(MSVC)
        target_compile_options(laya_benchmarks PRIVATE /W4)
    else()
        target_compile_options(laya_benchmarks PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    # Copy SDL shared libraries to benchmark executable directory for runtime
    laya_copy_sdl_shared_libs(laya_benchmarks)

    add_test(NAME laya_benchmarks COMMAND laya_benchmarks --test-suite=benchmark)
endif()
