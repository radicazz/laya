/// @file test_window.cpp
/// @brief Unit tests for window wrapper
/// @date 2025-12-12

#include <doctest/doctest.h>
#include <laya/laya.hpp>
#include <SDL3/SDL.h>

namespace {

void set_headless_video_driver() {
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
}

}  // namespace

TEST_SUITE("window") {
    TEST_CASE("basic lifecycle and size queries") {
        set_headless_video_driver();
        laya::context ctx(laya::subsystem::video);

        laya::window win("Unit Test Window", {320, 240});
        CHECK(win.is_valid());
        CHECK(win.id().is_valid());
        auto size = win.get_size();
        CHECK(size.width == 320);
        CHECK(size.height == 240);

        CHECK_NOTHROW(win.set_title("renamed window"));
        CHECK_NOTHROW(win.show());
        CHECK_NOTHROW(win.hide());

        CHECK_NOTHROW(win.set_resizable(true));
        CHECK_NOTHROW(win.set_borderless(false));
        CHECK_NOTHROW(win.set_minimum_size({100, 80}));
        CHECK_NOTHROW(win.set_maximum_size({640, 480}));

        auto min_size = win.get_minimum_size();
        CHECK(min_size.width == 100);
        CHECK(min_size.height == 80);

        auto max_size = win.get_maximum_size();
        CHECK(max_size.width == 640);
        CHECK(max_size.height == 480);
    }

    TEST_CASE("moved-from window throws on use") {
        set_headless_video_driver();
        laya::context ctx(laya::subsystem::video);

        laya::window first("Movable Window", {200, 150});
        laya::window second(std::move(first));

        CHECK(second.is_valid());
        CHECK_FALSE(first.is_valid());
        CHECK_THROWS_AS(first.show(), laya::error);
        CHECK_THROWS_AS(first.set_size({100, 100}), laya::error);
    }
}
