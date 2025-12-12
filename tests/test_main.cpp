#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <SDL3/SDL.h>

/// Global test fixture used for process-wide SDL hints.
/// Sets dummy drivers so unit tests run headless/non-interactive across CI.
struct global_test_context {
    global_test_context() {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy");
    }

    ~global_test_context() = default;
};

global_test_context g_test_setup;
