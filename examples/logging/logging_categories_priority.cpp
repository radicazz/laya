#include <laya/laya.hpp>

#include "example_base.hpp"

int main(int argc, char* argv[]) {
    auto settings = laya_examples::parse_settings(argc, argv);

    laya::log_info("Demonstrating category and priority control");

    laya::log(laya::log_category::video, laya::log_priority::info, "Video subsystem initialized");
    laya::log(laya::log_category::audio, laya::log_priority::debug, "Audio buffer size: {}", 2048);

    {
        laya::log_info("Temporarily enabling verbose render logging");
        auto guard = laya::with_log_priority(laya::log_category::render, laya::log_priority::verbose);
        laya::log(laya::log_category::render, laya::log_priority::verbose, "Verbose render detail");
        laya::log(laya::log_category::render, laya::log_priority::debug, "Renderer created");
    }

    laya::log_info("Render priority restored");

    if (settings.ci_mode) {
        laya::log_info("CI mode active");
    }

    return 0;
}
