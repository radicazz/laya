/// @file test_window_event.cpp
/// @brief Unit tests for type-safe window events

#include <doctest/doctest.h>
#include <laya/events/event_window.hpp>
#include <laya/windows/window_id.hpp>

TEST_CASE("window_event_data_none construction") {
    laya::window_event_data_none data;
    CHECK(std::holds_alternative<laya::window_event_data_none>(laya::window_event_data{data}));
}

TEST_CASE("window_event_data_position construction and access") {
    laya::window_event_data_position pos{100, 200};
    CHECK(pos.x == 100);
    CHECK(pos.y == 200);

    laya::window_event_data data = pos;
    CHECK(std::holds_alternative<laya::window_event_data_position>(data));

    auto* pos_ptr = std::get_if<laya::window_event_data_position>(&data);
    REQUIRE(pos_ptr != nullptr);
    CHECK(pos_ptr->x == 100);
    CHECK(pos_ptr->y == 200);
}

TEST_CASE("window_event_data_size construction and access") {
    laya::window_event_data_size size{800, 600};
    CHECK(size.width == 800);
    CHECK(size.height == 600);

    laya::window_event_data data = size;
    CHECK(std::holds_alternative<laya::window_event_data_size>(data));

    auto* size_ptr = std::get_if<laya::window_event_data_size>(&data);
    REQUIRE(size_ptr != nullptr);
    CHECK(size_ptr->width == 800);
    CHECK(size_ptr->height == 600);
}

TEST_CASE("window_event_data_display construction and access") {
    laya::window_event_data_display display{1};
    CHECK(display.display_index == 1);

    laya::window_event_data data = display;
    CHECK(std::holds_alternative<laya::window_event_data_display>(data));

    auto* display_ptr = std::get_if<laya::window_event_data_display>(&data);
    REQUIRE(display_ptr != nullptr);
    CHECK(display_ptr->display_index == 1);
}

TEST_CASE("window_event construction with different data types") {
    SUBCASE("event with no data") {
        laya::window_event event{};
        event.timestamp = 1000;
        event.id = laya::window_id{42};
        event.event_type = laya::window_event_type::shown;
        event.data = laya::window_event_data_none{};

        CHECK(event.timestamp == 1000);
        CHECK(event.id.value() == 42);
        CHECK(event.event_type == laya::window_event_type::shown);
        CHECK(std::holds_alternative<laya::window_event_data_none>(event.data));
    }

    SUBCASE("event with position data") {
        laya::window_event event{};
        event.timestamp = 2000;
        event.id = laya::window_id{43};
        event.event_type = laya::window_event_type::moved;
        event.data = laya::window_event_data_position{150, 250};

        CHECK(event.timestamp == 2000);
        CHECK(event.id.value() == 43);
        CHECK(event.event_type == laya::window_event_type::moved);

        auto* pos = std::get_if<laya::window_event_data_position>(&event.data);
        REQUIRE(pos != nullptr);
        CHECK(pos->x == 150);
        CHECK(pos->y == 250);
    }

    SUBCASE("event with size data") {
        laya::window_event event{};
        event.timestamp = 3000;
        event.id = laya::window_id{44};
        event.event_type = laya::window_event_type::resized;
        event.data = laya::window_event_data_size{1024, 768};

        CHECK(event.timestamp == 3000);
        CHECK(event.id.value() == 44);
        CHECK(event.event_type == laya::window_event_type::resized);

        auto* size = std::get_if<laya::window_event_data_size>(&event.data);
        REQUIRE(size != nullptr);
        CHECK(size->width == 1024);
        CHECK(size->height == 768);
    }

    SUBCASE("event with display data") {
        laya::window_event event{};
        event.timestamp = 4000;
        event.id = laya::window_id{45};
        event.event_type = laya::window_event_type::display_changed;
        event.data = laya::window_event_data_display{2};

        CHECK(event.timestamp == 4000);
        CHECK(event.id.value() == 45);
        CHECK(event.event_type == laya::window_event_type::display_changed);

        auto* display = std::get_if<laya::window_event_data_display>(&event.data);
        REQUIRE(display != nullptr);
        CHECK(display->display_index == 2);
    }
}

TEST_CASE("get_position helper function") {
    SUBCASE("returns position for moved event") {
        laya::window_event event{};
        event.event_type = laya::window_event_type::moved;
        event.data = laya::window_event_data_position{100, 200};

        auto pos = laya::get_position(event);
        REQUIRE(pos.has_value());
        CHECK(pos->x == 100);
        CHECK(pos->y == 200);
    }

    SUBCASE("returns nullopt for non-moved event") {
        laya::window_event event{};
        event.event_type = laya::window_event_type::shown;
        event.data = laya::window_event_data_none{};

        auto pos = laya::get_position(event);
        CHECK_FALSE(pos.has_value());
    }

    SUBCASE("returns nullopt for resized event") {
        laya::window_event event{};
        event.event_type = laya::window_event_type::resized;
        event.data = laya::window_event_data_size{800, 600};

        auto pos = laya::get_position(event);
        CHECK_FALSE(pos.has_value());
    }
}

TEST_CASE("get_size helper function") {
    SUBCASE("returns size for resized event") {
        laya::window_event event{};
        event.event_type = laya::window_event_type::resized;
        event.data = laya::window_event_data_size{800, 600};

        auto size = laya::get_size(event);
        REQUIRE(size.has_value());
        CHECK(size->width == 800);
        CHECK(size->height == 600);
    }

    SUBCASE("returns size for size_changed event") {
        laya::window_event event{};
        event.event_type = laya::window_event_type::size_changed;
        event.data = laya::window_event_data_size{1024, 768};

        auto size = laya::get_size(event);
        REQUIRE(size.has_value());
        CHECK(size->width == 1024);
        CHECK(size->height == 768);
    }

    SUBCASE("returns nullopt for non-size event") {
        laya::window_event event{};
        event.event_type = laya::window_event_type::shown;
        event.data = laya::window_event_data_none{};

        auto size = laya::get_size(event);
        CHECK_FALSE(size.has_value());
    }

    SUBCASE("returns nullopt for moved event") {
        laya::window_event event{};
        event.event_type = laya::window_event_type::moved;
        event.data = laya::window_event_data_position{100, 200};

        auto size = laya::get_size(event);
        CHECK_FALSE(size.has_value());
    }
}

TEST_CASE("get_display helper function") {
    SUBCASE("returns display for display_changed event") {
        laya::window_event event{};
        event.event_type = laya::window_event_type::display_changed;
        event.data = laya::window_event_data_display{3};

        auto display = laya::get_display(event);
        REQUIRE(display.has_value());
        CHECK(display->display_index == 3);
    }

    SUBCASE("returns nullopt for non-display_changed event") {
        laya::window_event event{};
        event.event_type = laya::window_event_type::shown;
        event.data = laya::window_event_data_none{};

        auto display = laya::get_display(event);
        CHECK_FALSE(display.has_value());
    }

    SUBCASE("returns nullopt for resized event") {
        laya::window_event event{};
        event.event_type = laya::window_event_type::resized;
        event.data = laya::window_event_data_size{800, 600};

        auto display = laya::get_display(event);
        CHECK_FALSE(display.has_value());
    }
}

TEST_CASE("window_event_type enum values") {
    // Ensure all enum values are distinct
    CHECK(laya::window_event_type::shown != laya::window_event_type::hidden);
    CHECK(laya::window_event_type::moved != laya::window_event_type::resized);
    CHECK(laya::window_event_type::minimized != laya::window_event_type::maximized);
    CHECK(laya::window_event_type::focus_gained != laya::window_event_type::focus_lost);
    CHECK(laya::window_event_type::enter != laya::window_event_type::leave);
}

TEST_CASE("std::visit with window_event_data") {
    SUBCASE("visit position data") {
        laya::window_event_data data = laya::window_event_data_position{50, 75};

        bool visited = false;
        std::visit(
            [&](const auto& d) {
                using T = std::decay_t<decltype(d)>;
                if constexpr (std::is_same_v<T, laya::window_event_data_position>) {
                    CHECK(d.x == 50);
                    CHECK(d.y == 75);
                    visited = true;
                }
            },
            data);

        CHECK(visited);
    }

    SUBCASE("visit size data") {
        laya::window_event_data data = laya::window_event_data_size{640, 480};

        bool visited = false;
        std::visit(
            [&](const auto& d) {
                using T = std::decay_t<decltype(d)>;
                if constexpr (std::is_same_v<T, laya::window_event_data_size>) {
                    CHECK(d.width == 640);
                    CHECK(d.height == 480);
                    visited = true;
                }
            },
            data);

        CHECK(visited);
    }

    SUBCASE("visit display data") {
        laya::window_event_data data = laya::window_event_data_display{1};

        bool visited = false;
        std::visit(
            [&](const auto& d) {
                using T = std::decay_t<decltype(d)>;
                if constexpr (std::is_same_v<T, laya::window_event_data_display>) {
                    CHECK(d.display_index == 1);
                    visited = true;
                }
            },
            data);

        CHECK(visited);
    }

    SUBCASE("visit none data") {
        laya::window_event_data data = laya::window_event_data_none{};

        bool visited = false;
        std::visit(
            [&](const auto& d) {
                using T = std::decay_t<decltype(d)>;
                if constexpr (std::is_same_v<T, laya::window_event_data_none>) {
                    visited = true;
                }
            },
            data);

        CHECK(visited);
    }
}

TEST_CASE("window_event type safety - compile-time guarantees") {
    // This test verifies that we can construct events with correct types
    // and that the variant holds the expected type

    SUBCASE("cannot accidentally assign wrong data to event type") {
        laya::window_event event{};
        event.event_type = laya::window_event_type::shown;
        event.data = laya::window_event_data_none{};  // Correct

        // Verify we can check the variant type
        CHECK(std::holds_alternative<laya::window_event_data_none>(event.data));
    }

    SUBCASE("variant can hold any of the four types") {
        laya::window_event_data data1 = laya::window_event_data_none{};
        CHECK(std::holds_alternative<laya::window_event_data_none>(data1));

        laya::window_event_data data2 = laya::window_event_data_position{0, 0};
        CHECK(std::holds_alternative<laya::window_event_data_position>(data2));

        laya::window_event_data data3 = laya::window_event_data_size{0, 0};
        CHECK(std::holds_alternative<laya::window_event_data_size>(data3));

        laya::window_event_data data4 = laya::window_event_data_display{0};
        CHECK(std::holds_alternative<laya::window_event_data_display>(data4));
    }
}
