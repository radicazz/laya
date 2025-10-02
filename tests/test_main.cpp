#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

//
// Currently unused, but can be helpful for global setup/teardown if needed in the future
//

struct global_test_context {
    global_test_context() {
    }

    ~global_test_context() {
    }
};

global_test_context g_test_setup;
