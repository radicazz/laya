# Logging

Type-safe logging with `std::format` support.

## Basic Logging

```cpp
laya::log_trace("Very verbose");
laya::log_verbose("Verbose");
laya::log_debug("Debug info");
laya::log_info("Information");
laya::log_warn("Warning");
laya::log_error("Error");
laya::log_critical("Critical");
```

With formatting:
```cpp
laya::log_info("Window size: {}x{}", 800, 600);
laya::log_debug("Frame time: {:.2f}ms", frame_time);
```

Format strings are checked at compile time.

## Categories

Log to specific subsystems:

```cpp
laya::log(laya::log_category::video, laya::log_priority::info,
          "Video initialized");

laya::log(laya::log_category::audio, laya::log_priority::debug,
          "Audio buffer: {}", buffer_size);
```

Available categories: `application`, `error`, `system`, `audio`, `video`, `render`, `input`, `gpu`

## Priority Control

Set minimum priority levels:
```cpp
laya::set_log_priority(laya::log_category::render, laya::log_priority::warn);
laya::set_all_log_priorities(laya::log_priority::info);
laya::reset_log_priorities();  // Back to defaults
```

## Example

```cpp
laya::context ctx{laya::subsystem::video};
laya::log_info("Initialized");

laya::window win{"App", {800, 600}};
laya::log_debug("Window created: {}x{}", 800, 600);

laya::renderer ren{win};
laya::log(laya::log_category::render, laya::log_priority::info, "Renderer ready");
```

```
