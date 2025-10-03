/// @file test_window_event_conversion.cpp
/// @brief Integration tests for window event conversion from SDL

#include <doctest/doctest.h>
#include <laya/events/event_types.hpp>
#include <laya/subsystems.hpp>
#include <SDL3/SDL.h>

TEST_CASE("SDL window event conversion - moved event") {
    // Initialize SDL for testing
    laya::context ctx(laya::subsystem::video | laya::subsystem::events);

    SDL_Event sdl_ev{};
    sdl_ev.type = SDL_EVENT_WINDOW_MOVED;
    sdl_ev.window.timestamp = 1000;
    sdl_ev.window.windowID = 42;
    sdl_ev.window.data1 = 100;
    sdl_ev.window.data2 = 200;

    auto laya_event = laya::from_sdl_event(sdl_ev);

    REQUIRE(std::holds_alternative<laya::window_event>(laya_event));

    const auto& win_event = std::get<laya::window_event>(laya_event);
    CHECK(win_event.timestamp == 1000);
    CHECK(win_event.id.value() == 42);
    CHECK(win_event.event_type == laya::window_event_type::moved);

    // Verify position data is correctly stored
    CHECK(std::holds_alternative<laya::window_event_data_position>(win_event.data));

    auto pos = laya::get_position(win_event);
    REQUIRE(pos.has_value());
    CHECK(pos->x == 100);
    CHECK(pos->y == 200);
}

TEST_CASE("SDL window event conversion - resized event") {
    laya::context ctx(laya::subsystem::video | laya::subsystem::events);

    SDL_Event sdl_ev{};
    sdl_ev.type = SDL_EVENT_WINDOW_RESIZED;
    sdl_ev.window.timestamp = 2000;
    sdl_ev.window.windowID = 43;
    sdl_ev.window.data1 = 800;
    sdl_ev.window.data2 = 600;

    auto laya_event = laya::from_sdl_event(sdl_ev);

    REQUIRE(std::holds_alternative<laya::window_event>(laya_event));

    const auto& win_event = std::get<laya::window_event>(laya_event);
    CHECK(win_event.timestamp == 2000);
    CHECK(win_event.id.value() == 43);
    CHECK(win_event.event_type == laya::window_event_type::resized);

    // Verify size data is correctly stored
    CHECK(std::holds_alternative<laya::window_event_data_size>(win_event.data));

    auto size = laya::get_size(win_event);
    REQUIRE(size.has_value());
    CHECK(size->width == 800);
    CHECK(size->height == 600);
}

TEST_CASE("SDL window event conversion - size_changed event") {
    laya::context ctx(laya::subsystem::video | laya::subsystem::events);

    SDL_Event sdl_ev{};
    sdl_ev.type = SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED;
    sdl_ev.window.timestamp = 3000;
    sdl_ev.window.windowID = 44;
    sdl_ev.window.data1 = 1024;
    sdl_ev.window.data2 = 768;

    auto laya_event = laya::from_sdl_event(sdl_ev);

    REQUIRE(std::holds_alternative<laya::window_event>(laya_event));

    const auto& win_event = std::get<laya::window_event>(laya_event);
    CHECK(win_event.timestamp == 3000);
    CHECK(win_event.id.value() == 44);
    CHECK(win_event.event_type == laya::window_event_type::size_changed);

    // Verify size data is correctly stored
    CHECK(std::holds_alternative<laya::window_event_data_size>(win_event.data));

    auto size = laya::get_size(win_event);
    REQUIRE(size.has_value());
    CHECK(size->width == 1024);
    CHECK(size->height == 768);
}

TEST_CASE("SDL window event conversion - display_changed event") {
    laya::context ctx(laya::subsystem::video | laya::subsystem::events);

    SDL_Event sdl_ev{};
    sdl_ev.type = SDL_EVENT_WINDOW_DISPLAY_CHANGED;
    sdl_ev.window.timestamp = 4000;
    sdl_ev.window.windowID = 45;
    sdl_ev.window.data1 = 2;
    sdl_ev.window.data2 = 0;  // Unused

    auto laya_event = laya::from_sdl_event(sdl_ev);

    REQUIRE(std::holds_alternative<laya::window_event>(laya_event));

    const auto& win_event = std::get<laya::window_event>(laya_event);
    CHECK(win_event.timestamp == 4000);
    CHECK(win_event.id.value() == 45);
    CHECK(win_event.event_type == laya::window_event_type::display_changed);

    // Verify display data is correctly stored
    CHECK(std::holds_alternative<laya::window_event_data_display>(win_event.data));

    auto display = laya::get_display(win_event);
    REQUIRE(display.has_value());
    CHECK(display->display_index == 2);
}

TEST_CASE("SDL window event conversion - events with no data") {
    laya::context ctx(laya::subsystem::video | laya::subsystem::events);

    SUBCASE("shown event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_SHOWN;
        sdl_ev.window.timestamp = 5000;
        sdl_ev.window.windowID = 46;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        REQUIRE(std::holds_alternative<laya::window_event>(laya_event));

        const auto& win_event = std::get<laya::window_event>(laya_event);
        CHECK(win_event.event_type == laya::window_event_type::shown);
        CHECK(std::holds_alternative<laya::window_event_data_none>(win_event.data));
    }

    SUBCASE("hidden event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_HIDDEN;
        sdl_ev.window.windowID = 47;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);
        CHECK(win_event.event_type == laya::window_event_type::hidden);
        CHECK(std::holds_alternative<laya::window_event_data_none>(win_event.data));
    }

    SUBCASE("exposed event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_EXPOSED;
        sdl_ev.window.windowID = 48;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);
        CHECK(win_event.event_type == laya::window_event_type::exposed);
        CHECK(std::holds_alternative<laya::window_event_data_none>(win_event.data));
    }

    SUBCASE("minimized event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_MINIMIZED;
        sdl_ev.window.windowID = 49;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);
        CHECK(win_event.event_type == laya::window_event_type::minimized);
        CHECK(std::holds_alternative<laya::window_event_data_none>(win_event.data));
    }

    SUBCASE("maximized event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_MAXIMIZED;
        sdl_ev.window.windowID = 50;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);
        CHECK(win_event.event_type == laya::window_event_type::maximized);
        CHECK(std::holds_alternative<laya::window_event_data_none>(win_event.data));
    }

    SUBCASE("restored event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_RESTORED;
        sdl_ev.window.windowID = 51;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);
        CHECK(win_event.event_type == laya::window_event_type::restored);
        CHECK(std::holds_alternative<laya::window_event_data_none>(win_event.data));
    }

    SUBCASE("mouse_enter event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_MOUSE_ENTER;
        sdl_ev.window.windowID = 52;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);
        CHECK(win_event.event_type == laya::window_event_type::enter);
        CHECK(std::holds_alternative<laya::window_event_data_none>(win_event.data));
    }

    SUBCASE("mouse_leave event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_MOUSE_LEAVE;
        sdl_ev.window.windowID = 53;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);
        CHECK(win_event.event_type == laya::window_event_type::leave);
        CHECK(std::holds_alternative<laya::window_event_data_none>(win_event.data));
    }

    SUBCASE("focus_gained event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_FOCUS_GAINED;
        sdl_ev.window.windowID = 54;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);
        CHECK(win_event.event_type == laya::window_event_type::focus_gained);
        CHECK(std::holds_alternative<laya::window_event_data_none>(win_event.data));
    }

    SUBCASE("focus_lost event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_FOCUS_LOST;
        sdl_ev.window.windowID = 55;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);
        CHECK(win_event.event_type == laya::window_event_type::focus_lost);
        CHECK(std::holds_alternative<laya::window_event_data_none>(win_event.data));
    }

    SUBCASE("close event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
        sdl_ev.window.windowID = 56;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);
        CHECK(win_event.event_type == laya::window_event_type::close);
        CHECK(std::holds_alternative<laya::window_event_data_none>(win_event.data));
    }
}

TEST_CASE("window event helper functions return nullopt for wrong event types") {
    laya::context ctx(laya::subsystem::video | laya::subsystem::events);

    // Create a shown event (no data)
    SDL_Event sdl_ev{};
    sdl_ev.type = SDL_EVENT_WINDOW_SHOWN;
    sdl_ev.window.windowID = 100;

    auto laya_event = laya::from_sdl_event(sdl_ev);
    const auto& win_event = std::get<laya::window_event>(laya_event);

    // All helper functions should return nullopt for shown event
    CHECK_FALSE(laya::get_position(win_event).has_value());
    CHECK_FALSE(laya::get_size(win_event).has_value());
    CHECK_FALSE(laya::get_display(win_event).has_value());
}

TEST_CASE("window event data can be pattern-matched with std::visit") {
    laya::context ctx(laya::subsystem::video | laya::subsystem::events);

    SUBCASE("visit moved event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_MOVED;
        sdl_ev.window.windowID = 200;
        sdl_ev.window.data1 = 150;
        sdl_ev.window.data2 = 250;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);

        bool visited_position = false;
        std::visit(
            [&](const auto& data) {
                using T = std::decay_t<decltype(data)>;
                if constexpr (std::is_same_v<T, laya::window_event_data_position>) {
                    CHECK(data.x == 150);
                    CHECK(data.y == 250);
                    visited_position = true;
                }
            },
            win_event.data);

        CHECK(visited_position);
    }

    SUBCASE("visit resized event") {
        SDL_Event sdl_ev{};
        sdl_ev.type = SDL_EVENT_WINDOW_RESIZED;
        sdl_ev.window.windowID = 201;
        sdl_ev.window.data1 = 1280;
        sdl_ev.window.data2 = 720;

        auto laya_event = laya::from_sdl_event(sdl_ev);
        const auto& win_event = std::get<laya::window_event>(laya_event);

        bool visited_size = false;
        std::visit(
            [&](const auto& data) {
                using T = std::decay_t<decltype(data)>;
                if constexpr (std::is_same_v<T, laya::window_event_data_size>) {
                    CHECK(data.width == 1280);
                    CHECK(data.height == 720);
                    visited_size = true;
                }
            },
            win_event.data);

        CHECK(visited_size);
    }
}
