#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// Global test configuration
namespace {
struct global_test_context {
    global_test_context() {
        // Global test setup if needed
    }

    ~global_test_context() {
        // Global test cleanup if needed
    }
};

global_test_context g_test_setup;
}  // namespace
