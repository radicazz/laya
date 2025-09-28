#include <cstring>

#include <doctest/doctest.h>
#include <laya/laya.hpp>
#include <SDL3/SDL.h>

#include "../utils/test_helpers.hpp"

TEST_SUITE("integration") {
    LAYA_TEST_WITH_SDL("SDL error state is clean after init")
    // Verify that SDL error state is clean after successful initialization
    CHECK(laya::sdl::error_view().empty() == true);
}

LAYA_TEST_WITH_SDL("SDL basic functionality works")
// Test that basic SDL functions work after laya::init()
// This verifies that SDL is properly initialized
CHECK(laya::sdl::platform_view().empty() == false);
MESSAGE("Platform: ", std::string(laya::sdl::platform_view()));
}

TEST_CASE("init failure handling consistency") {
    // Test that multiple init/quit cycles maintain consistent state
    // This is important for applications that might restart SDL

    for (int i = 0; i < 3; ++i) {
        CHECK_NOTHROW(laya::create(laya::subsystem::video));

        // Verify SDL is working by checking platform
        CHECK(laya::sdl::platform_view().empty() == false);
    }

    CHECK_NOTHROW(laya::destroy());
}
}
