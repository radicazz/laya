#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// Global test configuration
namespace {
    struct GlobalTestSetup {
        GlobalTestSetup() {
            // Global test setup if needed
        }

        ~GlobalTestSetup() {
            // Global test cleanup if needed
        }
    };

    GlobalTestSetup g_test_setup;
}  // namespace
