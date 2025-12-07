#include <laya/laya.hpp>

#include "example_base.hpp"

int main(int argc, char* argv[]) {
    auto settings = laya_examples::parse_settings(argc, argv);

    laya::log_info("Starting logging basics demo");

    laya::log_trace("Trace message");
    laya::log_verbose("Verbose message");
    laya::log_debug("Debug message");
    laya::log_info("Info message");
    laya::log_warn("Warning message");
    laya::log_error("Error message");
    laya::log_critical("Critical message");

    laya::log_info("Formatted size: {}x{}", 800, 600);
    laya::log_debug("Frame time: {:.2f}ms", 16.67);

    if (settings.ci_mode) {
        laya::log_info("CI mode active");
    }

    return 0;
}
