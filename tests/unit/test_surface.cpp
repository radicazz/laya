/// @file test_surface.cpp
/// @brief Basic unit tests for surface API
/// @date 2025-12-10

#include <laya/laya.hpp>
#include <doctest/doctest.h>

using namespace laya;

TEST_SUITE("Surface") {
    // Helper function to create a test surface
    auto create_test_surface = [](dimentions size = {64, 64}, pixel_format fmt = pixel_format::rgba32) {
        return surface{size, fmt};
    };

    TEST_CASE("Surface creation - Basic surface creation") {
        laya::context ctx{laya::subsystem::video};
        auto surf = create_test_surface();

        CHECK(surf.size().width == 64);
        CHECK(surf.size().height == 64);
        CHECK(surf.format() == pixel_format::rgba32);
        CHECK(surf.native_handle() != nullptr);
    }

    TEST_CASE("Surface creation - Surface creation with args") {
        laya::context ctx{laya::subsystem::video};
        surface_args args{.size = {128, 256}, .format = pixel_format::bgra32, .flags = surface_flags::rle_optimized};

        laya::surface surf{args};

        CHECK(surf.size().width == 128);
        CHECK(surf.size().height == 256);
        CHECK(surf.format() == pixel_format::bgra32);
    }

    TEST_CASE("Surface operations - Fill operations") {
        laya::context ctx{laya::subsystem::video};
        auto surf = create_test_surface({32, 32});

        // These should not throw
        CHECK_NOTHROW(surf.fill(laya::colors::red));
        CHECK_NOTHROW(surf.fill_rect({0, 0, 16, 16}, laya::colors::blue));
        CHECK_NOTHROW(surf.clear());
    }

    TEST_CASE("Surface operations - State management") {
        laya::context ctx{laya::subsystem::video};
        auto surf = create_test_surface({32, 32});

        // These should not throw
        CHECK_NOTHROW(surf.set_alpha_mod(128));
        CHECK_NOTHROW(surf.set_color_mod(laya::colors::green));
        CHECK_NOTHROW(surf.set_blend_mode(laya::blend_mode::blend));

        // Verify getters work
        CHECK(surf.get_alpha_mod() == 128);
        CHECK(surf.get_color_mod() == laya::colors::green);
        CHECK(surf.get_blend_mode() == laya::blend_mode::blend);
    }

    TEST_CASE("Surface operations - Color key operations") {
        laya::context ctx{laya::subsystem::video};
        auto surf = create_test_surface({32, 32});

        CHECK_FALSE(surf.has_color_key());

        CHECK_NOTHROW(surf.set_color_key(laya::colors::magenta));
        CHECK(surf.has_color_key());
        CHECK(surf.get_color_key() == laya::colors::magenta);

        CHECK_NOTHROW(surf.clear_color_key());
        CHECK_FALSE(surf.has_color_key());
    }

    TEST_CASE("Surface transformations - Duplicate") {
        laya::context ctx{laya::subsystem::video};
        auto surf = create_test_surface({16, 16});

        auto dup = surf.duplicate();
        CHECK(dup.size().width == surf.size().width);
        CHECK(dup.size().height == surf.size().height);
        CHECK(dup.format() == surf.format());
        CHECK(dup.native_handle() != surf.native_handle());  // Different objects
    }

    TEST_CASE("Surface transformations - Convert format") {
        laya::context ctx{laya::subsystem::video};
        auto surf = create_test_surface({16, 16});

        auto converted = surf.convert(pixel_format::bgra32);
        CHECK(converted.size().width == surf.size().width);
        CHECK(converted.size().height == surf.size().height);
        CHECK(converted.format() == pixel_format::bgra32);
    }

    TEST_CASE("Surface transformations - Scale") {
        laya::context ctx{laya::subsystem::video};
        auto surf = create_test_surface({16, 16});

        auto scaled = surf.scale({32, 32});
        CHECK(scaled.size().width == 32);
        CHECK(scaled.size().height == 32);
        CHECK(scaled.format() == surf.format());
    }

    TEST_CASE("Surface transformations - Flip") {
        laya::context ctx{laya::subsystem::video};
        auto surf = create_test_surface({16, 16});

        auto flipped_h = surf.flip(flip_mode::horizontal);
        auto flipped_v = surf.flip(flip_mode::vertical);
        auto flipped_none = surf.flip(flip_mode::none);

        // All should have same size and format
        CHECK(flipped_h.size().width == surf.size().width);
        CHECK(flipped_h.size().height == surf.size().height);
        CHECK(flipped_v.size().width == surf.size().width);
        CHECK(flipped_v.size().height == surf.size().height);
        CHECK(flipped_none.size().width == surf.size().width);
        CHECK(flipped_none.size().height == surf.size().height);

        CHECK(flipped_h.format() == surf.format());
        CHECK(flipped_v.format() == surf.format());
        CHECK(flipped_none.format() == surf.format());
    }

    TEST_CASE("Surface locking - Basic lock guard usage") {
        laya::context ctx{laya::subsystem::video};
        auto surf = create_test_surface({8, 8});

        {
            auto lock = surf.lock();
            CHECK(lock.pixels() != nullptr);
            CHECK(lock.pitch() > 0);
        }
        // Lock should be automatically released
    }

    TEST_CASE("Surface locking - Lock guard move semantics") {
        laya::context ctx{laya::subsystem::video};
        auto surf = create_test_surface({8, 8});

        auto lock1 = surf.lock();
        void* original_pixels = lock1.pixels();

        auto lock2 = std::move(lock1);
        CHECK(lock2.pixels() == original_pixels);
        // lock1 should be empty after move
    }

    TEST_CASE("Surface metadata") {
        laya::context ctx{laya::subsystem::video};
        auto surf = create_test_surface({42, 24}, pixel_format::rgb24);

        CHECK(surf.size().width == 42);
        CHECK(surf.size().height == 24);
        CHECK(surf.format() == pixel_format::rgb24);
        CHECK_FALSE(surf.must_lock());  // SDL3 typically doesn't require locking
    }
}
