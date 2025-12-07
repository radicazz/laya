#include <laya/laya.hpp>

#include <chrono>
#include <thread>

#include "example_base.hpp"

int main(int argc, char* argv[]) {
    auto settings = laya_examples::parse_settings(argc, argv);

    laya::log_info("Trying colored output (if supported)");

    if (settings.ci_mode) {
        laya::log_warn("CI mode: skipping color toggles");
        return 0;
    }

    if (laya::enable_log_colors()) {
        laya::log_info("Colors enabled");
        laya::log_trace("Trace: dim white");
        laya::log_verbose("Verbose: white");
        laya::log_debug("Debug: cyan");
        laya::log_info("Info: green");
        laya::log_warn("Warning: yellow");
        laya::log_error("Error: red");
        laya::log_critical("Critical: bold red");

        std::this_thread::sleep_for(std::chrono::milliseconds{250});
        laya::disable_log_colors();
        laya::log_info("Colors disabled");
    } else {
        laya::log_warn("Colored output not supported on this terminal");
    }

    return 0;
}
