#include "../utils/test_helpers.hpp"

TEST_SUITE("unit") {
    TEST_CASE("creating and destroying subsystems does not throw") {
        // Test that init and quit can be called without throwing
        CHECK_NOTHROW(laya::create(laya::subsystem::video));
        CHECK_NOTHROW(laya::destroy());
    }

    TEST_CASE("multiple init calls are safe") {
        // SDL3 should handle multiple init calls gracefully
        CHECK_NOTHROW(laya::create(laya::subsystem::video));
        CHECK_NOTHROW(laya::create(laya::subsystem::video));  // Second call should be safe
        CHECK_NOTHROW(laya::destroy());
        CHECK_NOTHROW(laya::destroy());  // Multiple quits should be safe
    }

    TEST_CASE("init after quit works") {
        CHECK_NOTHROW(laya::create(laya::subsystem::video));
        CHECK_NOTHROW(laya::destroy());
        CHECK_NOTHROW(laya::create(laya::subsystem::video));  // Should be able to reinitialize
        CHECK_NOTHROW(laya::destroy());
    }

    TEST_CASE("quit is noexcept") {
        // Verify quit is marked noexcept and doesn't throw
        static_assert(noexcept(laya::destroy()));

        laya::create(laya::subsystem::video);
        CHECK_NOTHROW(laya::destroy());
    }

}  // TEST_SUITE("unit")
