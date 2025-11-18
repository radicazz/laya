/// @file test_event_view.cpp
/// @brief Unit tests for event_view implementation

#include <doctest/doctest.h>
#include <laya/laya.hpp>

TEST_CASE("event_view basic functionality") {
    laya::context ctx(laya::subsystem::video);

    SUBCASE("event_view is default constructible") {
        laya::event_view view;
        // When no events, begin should equal end
        CHECK(view.begin() == view.end());
    }

    SUBCASE("events_view returns event_view") {
        auto view = laya::events_view();
        // Should compile and be iterable
        size_t count = 0;
        for ([[maybe_unused]] const auto& event : view) {
            ++count;
        }
        // No events expected in test environment
        CHECK(count == 0);
    }

    SUBCASE("event_view iterator has correct type traits") {
        using iterator = laya::event_view::iterator;
        using traits = std::iterator_traits<iterator>;

        CHECK(std::is_same_v<traits::value_type, laya::event>);
        CHECK(std::is_same_v<traits::iterator_category, std::input_iterator_tag>);
    }
}

TEST_CASE("event_view iterator operations") {
    laya::context ctx(laya::subsystem::video);

    SUBCASE("end iterator equals end iterator") {
        auto view = laya::events_view();
        auto end1 = view.end();
        auto end2 = view.end();
        CHECK(end1 == end2);
    }

    SUBCASE("iterator is copy constructible") {
        auto view = laya::events_view();
        auto it1 = view.begin();
        auto it2 = it1;  // Copy construction
        CHECK(it1 == it2);
    }
}

TEST_CASE("event_view noexcept specifications") {
    SUBCASE("events_view is noexcept") {
        CHECK(noexcept(laya::events_view()));
    }

    SUBCASE("end() is noexcept") {
        laya::event_view view;
        CHECK(noexcept(view.end()));
    }

    SUBCASE("iterator default constructor is noexcept") {
        CHECK(noexcept(laya::event_view::iterator()));
    }

    SUBCASE("iterator comparison is noexcept") {
        laya::event_view::iterator it1, it2;
        CHECK(noexcept(it1 == it2));
        CHECK(noexcept(it1 != it2));
    }
}
