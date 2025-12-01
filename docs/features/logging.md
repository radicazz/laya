# Logging

Laya provides a modern, type-safe logging system that wraps SDL3's logging functionality with C++20 features.

---

## Overview

The logging system provides:

- **Type-safe enums** for log categories and priorities
- **`std::format` support** for compile-time checked formatting
- **Automatic source location** for error and critical messages
- **RAII priority guards** for temporary priority changes
- **Cross-platform colored output** (optional feature)
- **Zero SDL exposure** in public headers

---

## Basic Usage

### Simple Logging

Seven priority levels matching SDL3:

```cpp
#include <laya/laya.hpp>

int main() {
    laya::log_trace("Very verbose debug info");
    laya::log_verbose("Verbose debug info");
    laya::log_debug("Debug message");
    laya::log_info("Informational message");
    laya::log_warn("Warning message");
    laya::log_error("Error message");       // Auto includes file:line
    laya::log_critical("Critical error");   // Auto includes file:line
    
    return 0;
}
```

### Format String Support

Uses C++20 `std::format` for type-safe formatting:

```cpp
laya::log_info("Window size: {}x{}", 800, 600);
laya::log_debug("Frame time: {:.2f}ms", frame_time);
laya::log_info("Player {}: score = {}", player_id, score);
```

The format string is checked at **compile time**, preventing runtime format errors.

### Automatic Source Location

Error and critical messages automatically include file and line information:

```cpp
laya::log_error("Renderer creation failed");
// Output: [/path/to/file.cpp:42] Renderer creation failed
```

This uses C++20's `std::source_location` automatically.

---

## Categories and Priorities

### Category-Specific Logging

Log to specific SDL categories:

```cpp
laya::log(laya::log_category::video, laya::log_priority::info, 
          "Video subsystem initialized");
          
laya::log(laya::log_category::audio, laya::log_priority::debug, 
          "Audio buffer size: {}", buffer_size);
```

### Available Categories

```cpp
enum class log_category {
    application,  // General application messages (default)
    error,        // Error messages
    assert_cat,   // Assertion failures
    system,       // System messages
    audio,        // Audio subsystem
    video,        // Video subsystem
    render,       // Rendering
    input,        // Input handling
    test,         // Test output
    gpu,          // GPU operations
    custom        // Custom messages
};
```

### Available Priorities

```cpp
enum class log_priority {
    trace,     // Very verbose debugging
    verbose,   // Verbose debugging
    debug,     // Debug information
    info,      // Informational messages
    warn,      // Warning messages
    error,     // Error messages
    critical   // Critical errors
};
```

---

## Priority Management

### Setting Priorities

Control which messages are shown per category:

```cpp
// Set minimum priority for a category
laya::set_log_priority(laya::log_category::render, 
                       laya::log_priority::verbose);

// Set priority for all categories
laya::set_all_log_priorities(laya::log_priority::warn);

// Get current priority
auto priority = laya::get_log_priority(laya::log_category::video);

// Reset to SDL defaults
laya::reset_log_priorities();
```

### RAII Priority Guards

Temporarily change priorities with automatic restoration:

```cpp
{
    auto guard = laya::with_log_priority(laya::log_category::render, 
                                          laya::log_priority::verbose);
    // Verbose render logs enabled here
    laya::log(laya::log_category::render, laya::log_priority::verbose,
              "Detailed render info");
}
// Priority automatically restored
```

---

## Colored Output

An optional feature that colors log messages by severity:

```cpp
if (laya::enable_log_colors()) {
    // Messages now colored by priority:
    // - Trace: dim white
    // - Verbose: white
    // - Debug: cyan
    // - Info: green
    // - Warn: yellow
    // - Error: red
    // - Critical: bold red
}

laya::disable_log_colors();  // Disable when done
```

### Platform Support

**Linux/macOS:**
- Uses ANSI escape codes
- Automatically checks if stderr is a TTY
- Returns `false` if output is redirected to file

**Windows:**
- Enables Virtual Terminal Processing on Windows 10+
- Falls back gracefully on older Windows versions
- Returns `false` if not supported

---

## Practical Examples

### Application Logging

```cpp
#include <laya/laya.hpp>

int main() {
    laya::context ctx{laya::subsystem::video};
    laya::log_info("Application started");
    
    laya::window win{"My App", {800, 600}};
    laya::log_info("Window created: ID={}", win.id().value());
    
    laya::renderer renderer{win};
    laya::log_debug("Renderer initialized");
    
    // ... application code ...
    
    laya::log_info("Application shutting down");
}
```

### Debug Logging During Development

```cpp
// Enable verbose logging during development
laya::set_all_log_priorities(laya::log_priority::verbose);

// Or just for specific subsystems
laya::set_log_priority(laya::log_category::render, 
                       laya::log_priority::verbose);
```

### Conditional Logging

```cpp
if (config.enable_debug_logging) {
    laya::enable_log_colors();
    laya::set_all_log_priorities(laya::log_priority::debug);
}
```

### Error Handling Integration

```cpp
try {
    laya::window win{"Test", {800, 600}};
} catch (const laya::error& e) {
    laya::log_critical("Window creation failed: {}", e.what());
    return 1;
}
```

---

## Performance Considerations

### Zero Overhead When Disabled

SDL3's logging system checks priority **before** formatting:

```cpp
laya::log_debug("Expensive: {}", compute_expensive_value());
// If debug priority is disabled, compute_expensive_value() is NOT called
```

This is handled by SDL internally - Laya doesn't add overhead.

### Compile-Time Format Checking

`std::format_string<Args...>` provides compile-time verification:

```cpp
laya::log_info("Value: {}", 42);           // ✅ OK
laya::log_info("Value: {}", 42, "extra");  // ❌ Compile error
laya::log_info("Value: {:d}", "not int");  // ❌ Compile error
```

No runtime format string parsing or validation needed.

---

## See Also

- **[Getting Started](../getting-started.md)** - Basic setup and usage
- **[Architecture](../architecture.md)** - Design patterns
- **[Examples](../../examples/logging/)** - Complete example code
