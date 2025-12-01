/// @file test_subsystems.cpp
/// @brief Unit tests for laya subsystem initialization
/// @date 2025-10-01

#include <iostream>

#include <doctest/doctest.h>
#include <laya/laya.hpp>

TEST_SUITE("unit") {
    TEST_CASE("context initialization with video subsystem") {
        // Test that creating a context with video subsystem doesn't throw
        CHECK_NOTHROW({
            laya::context ctx(laya::subsystem::video);
            // Context automatically cleans up in destructor
        });
    }

    TEST_CASE("context initialization with audio subsystem") {
        // Test that creating a context with audio subsystem doesn't throw
        CHECK_NOTHROW({ laya::context ctx(laya::subsystem::audio); });
    }

    TEST_CASE("context initialization with events subsystem") {
        // Test that creating a context with events subsystem doesn't throw
        CHECK_NOTHROW({ laya::context ctx(laya::subsystem::events); });
    }

    TEST_CASE("sequential context creation and destruction") {
        // Test that multiple sequential initializations work correctly
        {
            laya::context ctx1(laya::subsystem::video);
        }

        {
            laya::context ctx2(laya::subsystem::video);
        }
    }
}  // TEST_SUITE("unit")
