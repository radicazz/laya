/// @file test_logging.cpp
/// @brief Unit tests for laya logging system
/// @date 2025-11-18

#include <laya/laya.hpp>

#include <doctest/doctest.h>

TEST_SUITE("Logging") {
    // Initialize SDL for all logging tests
    TEST_CASE("Logging - Basic functions compile and run") {
        laya::context ctx{laya::subsystem::video};

        // Test all log levels
        CHECK_NOTHROW(laya::log_trace("Trace message"));
        CHECK_NOTHROW(laya::log_verbose("Verbose message"));
        CHECK_NOTHROW(laya::log_debug("Debug message"));
        CHECK_NOTHROW(laya::log_info("Info message"));
        CHECK_NOTHROW(laya::log_warn("Warning message"));
        CHECK_NOTHROW(laya::log_error("Error message"));
        CHECK_NOTHROW(laya::log_critical("Critical message"));
    }

    TEST_CASE("Logging - Format string support") {
        laya::context ctx{laya::subsystem::video};

        CHECK_NOTHROW(laya::log_info("Window size: {}x{}", 800, 600));
        CHECK_NOTHROW(laya::log_debug("Frame time: {:.2f}ms", 16.67));
        CHECK_NOTHROW(laya::log_info("Frame {}: {:.2f} FPS", 42, 59.94));
    }

    TEST_CASE("Logging - Category and priority") {
        laya::context ctx{laya::subsystem::video};

        CHECK_NOTHROW(laya::log(laya::log_category::video, laya::log_priority::info, "Video initialized"));
        CHECK_NOTHROW(laya::log(laya::log_category::audio, laya::log_priority::debug, "Audio buffer: {}", 2048));
        CHECK_NOTHROW(laya::log(laya::log_category::render, laya::log_priority::verbose, "Renderer created"));
    }

    TEST_CASE("Logging - Priority management") {
        laya::context ctx{laya::subsystem::video};

        // Set priority for a specific category
        CHECK_NOTHROW(laya::set_log_priority(laya::log_category::application, laya::log_priority::debug));

        // Get priority
        auto priority = laya::get_log_priority(laya::log_category::application);
        CHECK(priority == laya::log_priority::debug);

        // Reset priorities
        CHECK_NOTHROW(laya::reset_log_priorities());
    }

    TEST_CASE("Logging - Priority guard RAII") {
        laya::context ctx{laya::subsystem::video};

        auto original_priority = laya::get_log_priority(laya::log_category::render);

        {
            auto guard = laya::with_log_priority(laya::log_category::render, laya::log_priority::verbose);
            auto new_priority = laya::get_log_priority(laya::log_category::render);
            CHECK(new_priority == laya::log_priority::verbose);
        }

        // Priority should be restored
        auto restored_priority = laya::get_log_priority(laya::log_category::render);
        CHECK(restored_priority == original_priority);
    }

    TEST_CASE("Logging - Color support functions") {
        laya::context ctx{laya::subsystem::video};

        // Colors may or may not be supported depending on the terminal
        // Just check that the functions don't crash
        bool colors_enabled = laya::enable_log_colors();
        CHECK(laya::are_log_colors_enabled() == colors_enabled);

        laya::disable_log_colors();
        CHECK_FALSE(laya::are_log_colors_enabled());
    }

    TEST_CASE("Logging - Set all priorities") {
        laya::context ctx{laya::subsystem::video};

        CHECK_NOTHROW(laya::set_all_log_priorities(laya::log_priority::warn));

        // Verify a few categories
        CHECK(laya::get_log_priority(laya::log_category::application) == laya::log_priority::warn);
        CHECK(laya::get_log_priority(laya::log_category::video) == laya::log_priority::warn);

        // Reset to defaults
        laya::reset_log_priorities();
    }

    TEST_CASE("Logging - Source location in error/critical") {
        laya::context ctx{laya::subsystem::video};

        // These automatically include source location
        CHECK_NOTHROW(laya::log_error("Error with auto location"));
        CHECK_NOTHROW(laya::log_critical("Critical with auto location"));

        // Can also provide explicit location
        CHECK_NOTHROW(laya::log_error(std::source_location::current(), "Error with explicit location"));
        CHECK_NOTHROW(laya::log_critical(std::source_location::current(), "Critical with explicit location"));
    }

    TEST_CASE("Logging - Category-specific with location") {
        laya::context ctx{laya::subsystem::video};

        CHECK_NOTHROW(laya::log(laya::log_category::video, laya::log_priority::error, std::source_location::current(),
                                "Video error at location"));
    }
}
