#include <doctest/doctest.h>
#include <laya/init.hpp>
#include "../utils/test_helpers.hpp"
#include <SDL3/SDL.h>
#include <cstring>

TEST_SUITE("integration") {
    LAYA_TEST_WITH_SDL("SDL error state is clean after init")
    // Verify that SDL error state is clean after successful initialization
    const char* error = SDL_GetError();
    CHECK((error == nullptr || strlen(error) == 0));
}

LAYA_TEST_WITH_SDL("SDL basic functionality works")
// Test that basic SDL functions work after laya::init()
// This verifies that SDL is properly initialized
const char* platform = SDL_GetPlatform();
CHECK(platform != nullptr);
CHECK(strlen(platform) > 0);
MESSAGE("Platform: ", platform);
}

TEST_CASE("init failure handling consistency") {
    // Test that multiple init/quit cycles maintain consistent state
    // This is important for applications that might restart SDL

    for (int i = 0; i < 3; ++i) {
        CHECK_NOTHROW(laya::init());

        // Verify SDL is working by checking platform
        const char* platform = SDL_GetPlatform();
        CHECK(platform != nullptr);

        CHECK_NOTHROW(laya::quit());
    }
}

}  // TEST_SUITE("integration")
