/// @file test_renderer.cpp
/// @brief Unit tests for renderer functionality
/// @date 2025-10-07

#include <doctest/doctest.h>
#include <laya/laya.hpp>

TEST_SUITE("renderer") {
    TEST_CASE("renderer types") {
        SUBCASE("point construction") {
            laya::point p1;
            CHECK(p1.x == 0);
            CHECK(p1.y == 0);

            laya::point p2{10, 20};
            CHECK(p2.x == 10);
            CHECK(p2.y == 20);

            CHECK(p1 == laya::point{0, 0});
            CHECK(p2 == laya::point{10, 20});
            CHECK(p1 != p2);
        }

        SUBCASE("rect construction") {
            laya::rect r1;
            CHECK(r1.x == 0);
            CHECK(r1.y == 0);
            CHECK(r1.w == 0);
            CHECK(r1.h == 0);

            laya::rect r2{10, 20, 100, 200};
            CHECK(r2.x == 10);
            CHECK(r2.y == 20);
            CHECK(r2.w == 100);
            CHECK(r2.h == 200);

            CHECK(r2.position() == laya::point{10, 20});
            CHECK(r2.size() == laya::dimensions{100, 200});

            laya::rect r3{laya::point{5, 15}, laya::dimensions{50, 75}};
            CHECK(r3.x == 5);
            CHECK(r3.y == 15);
            CHECK(r3.w == 50);
            CHECK(r3.h == 75);
        }

        SUBCASE("color construction") {
            laya::color c1;
            CHECK(c1.r == 0);
            CHECK(c1.g == 0);
            CHECK(c1.b == 0);
            CHECK(c1.a == 255);

            laya::color c2{255, 128, 64};
            CHECK(c2.r == 255);
            CHECK(c2.g == 128);
            CHECK(c2.b == 64);
            CHECK(c2.a == 255);

            laya::color c3{255, 128, 64, 128};
            CHECK(c3.r == 255);
            CHECK(c3.g == 128);
            CHECK(c3.b == 64);
            CHECK(c3.a == 128);

            CHECK(c1 == laya::color{0, 0, 0, 255});
            CHECK(c2 == laya::color{255, 128, 64});
            CHECK(c1 != c2);
        }

        SUBCASE("color_f construction") {
            laya::color_f cf1;
            CHECK(cf1.r == 0.0f);
            CHECK(cf1.g == 0.0f);
            CHECK(cf1.b == 0.0f);
            CHECK(cf1.a == 1.0f);

            laya::color_f cf2{1.0f, 0.5f, 0.25f};
            CHECK(cf2.r == 1.0f);
            CHECK(cf2.g == 0.5f);
            CHECK(cf2.b == 0.25f);
            CHECK(cf2.a == 1.0f);

            laya::color_f cf3{1.0f, 0.5f, 0.25f, 0.5f};
            CHECK(cf3.r == 1.0f);
            CHECK(cf3.g == 0.5f);
            CHECK(cf3.b == 0.25f);
            CHECK(cf3.a == 0.5f);
        }

        SUBCASE("color constants") {
            CHECK(laya::colors::black == laya::color{0, 0, 0});
            CHECK(laya::colors::white == laya::color{255, 255, 255});
            CHECK(laya::colors::red == laya::color{255, 0, 0});
            CHECK(laya::colors::green == laya::color{0, 255, 0});
            CHECK(laya::colors::blue == laya::color{0, 0, 255});
            CHECK(laya::colors::transparent == laya::color{0, 0, 0, 0});
        }
    }

    TEST_CASE("renderer_flags enum") {
        using laya::renderer_flags;

        CHECK(static_cast<std::uint32_t>(renderer_flags::none) == 0x00000000u);
        CHECK(static_cast<std::uint32_t>(renderer_flags::software) == 0x00000001u);
        CHECK(static_cast<std::uint32_t>(renderer_flags::accelerated) == 0x00000002u);
        CHECK(static_cast<std::uint32_t>(renderer_flags::present_vsync) == 0x00000004u);
        CHECK(static_cast<std::uint32_t>(renderer_flags::target_texture) == 0x00000008u);

        // Note: Bitmask operations would be tested in integration tests
        // where the full bitmask.hpp functionality is available
    }

    TEST_CASE("blend_mode enum") {
        using laya::blend_mode;

        CHECK(static_cast<std::uint32_t>(blend_mode::none) == 0x00000000u);
        CHECK(static_cast<std::uint32_t>(blend_mode::blend) == 0x00000001u);
        CHECK(static_cast<std::uint32_t>(blend_mode::add) == 0x00000002u);
        CHECK(static_cast<std::uint32_t>(blend_mode::mod) == 0x00000004u);
        CHECK(static_cast<std::uint32_t>(blend_mode::mul) == 0x00000008u);
    }

    TEST_CASE("renderer_id") {
        laya::renderer_id id1;
        CHECK_FALSE(id1.is_valid());
        CHECK(id1.value() == 0);

        laya::renderer_id id2{42};
        CHECK(id2.is_valid());
        CHECK(id2.value() == 42);

        CHECK(id1 != id2);
        CHECK(id2 == laya::renderer_id{42});
    }

    // Note: Full renderer functionality tests would require SDL initialization
    // and window creation, which is more complex for unit tests.
    // Integration tests would be better suited for testing actual rendering.
}
