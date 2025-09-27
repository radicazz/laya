#include <doctest/doctest.h>
#include <laya/init.hpp>
#include "../utils/test_helpers.hpp"

TEST_SUITE("unit") {
    TEST_CASE("init and quit basic functionality") {
        // Test that init and quit can be called without throwing
        CHECK_NOTHROW(laya::init());
        CHECK_NOTHROW(laya::quit());
    }

    TEST_CASE("multiple init calls are safe") {
        // SDL3 should handle multiple init calls gracefully
        CHECK_NOTHROW(laya::init());
        CHECK_NOTHROW(laya::init());  // Second call should be safe
        CHECK_NOTHROW(laya::quit());
        CHECK_NOTHROW(laya::quit());  // Multiple quits should be safe
    }

    TEST_CASE("init after quit works") {
        CHECK_NOTHROW(laya::init());
        CHECK_NOTHROW(laya::quit());
        CHECK_NOTHROW(laya::init());  // Should be able to reinitialize
        CHECK_NOTHROW(laya::quit());
    }

    TEST_CASE("quit is noexcept") {
        // Verify quit is marked noexcept and doesn't throw
        static_assert(noexcept(laya::quit()));

        laya::init();
        CHECK_NOTHROW(laya::quit());
    }

}  // TEST_SUITE("unit")
