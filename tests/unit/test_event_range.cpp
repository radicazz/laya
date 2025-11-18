/// @file test_event_range.cpp
/// @brief Unit tests for event_range implementation

#include <doctest/doctest.h>
#include <laya/laya.hpp>

TEST_CASE("event_range basic functionality") {
    laya::context ctx(laya::subsystem::video);

    SUBCASE("event_range is constructible") {
        laya::event_range range;
        // When no events, range should be empty
        CHECK(range.empty());
        CHECK(range.size() == 0);
    }

    SUBCASE("events_range returns event_range") {
        auto range = laya::events_range();
        // Should compile and be iterable
        size_t count = 0;
        for ([[maybe_unused]] const auto& event : range) {
            ++count;
        }
        // No events expected in test environment
        CHECK(count == 0);
        CHECK(range.empty());
    }

    SUBCASE("event_range supports multi-pass iteration") {
        auto range = laya::events_range();

        // First pass
        size_t first_count = 0;
        for ([[maybe_unused]] const auto& event : range) {
            ++first_count;
        }

        // Second pass - should work with same result
        size_t second_count = 0;
        for ([[maybe_unused]] const auto& event : range) {
            ++second_count;
        }

        CHECK(first_count == second_count);
    }

    SUBCASE("event_range iterator has correct type traits") {
        using iterator = decltype(std::declval<laya::event_range>().begin());
        using traits = std::iterator_traits<iterator>;

        CHECK(std::is_same_v<typename traits::value_type, laya::event>);
    }
}

TEST_CASE("event_range iterator operations") {
    laya::context ctx(laya::subsystem::video);

    SUBCASE("begin equals end when empty") {
        auto range = laya::events_range();
        CHECK(range.begin() == range.end());
    }

    SUBCASE("iterator is copy constructible") {
        auto range = laya::events_range();
        auto it1 = range.begin();
        auto it2 = it1;  // Copy construction
        CHECK(it1 == it2);
    }
}

TEST_CASE("event_range noexcept specifications") {
    SUBCASE("begin() is noexcept") {
        laya::event_range range;
        CHECK(noexcept(range.begin()));
    }

    SUBCASE("end() is noexcept") {
        laya::event_range range;
        CHECK(noexcept(range.end()));
    }

    SUBCASE("empty() is noexcept") {
        laya::event_range range;
        CHECK(noexcept(range.empty()));
    }

    SUBCASE("size() is noexcept") {
        laya::event_range range;
        CHECK(noexcept(range.size()));
    }
}
