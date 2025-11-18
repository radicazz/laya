# Logging System Implementation

This document describes the design and implementation of laya's logging system, a modern C++ wrapper around SDL3's logging functionality.

---

## Overview

The logging system provides a type-safe, modern C++ interface to SDL3's logging capabilities with the following features:

- **Type-safe enums** for log categories and priorities
- **`std::format` support** for compile-time checked formatting
- **Automatic source location** for error and critical messages
- **RAII priority guards** for temporary priority changes
- **Cross-platform colored output** (optional feature)
- **Zero SDL exposure** in public headers

---

## Design Principles

### 1. Thin Wrapper Philosophy

The logging system stays true to laya's core principle of being a thin SDL3 wrapper:

- Direct mapping to SDL3 logging functions
- Minimal overhead - just type safety and convenience
- No additional abstractions beyond what SDL provides
- Optional QoL features (colors, source location) don't interfere with core functionality

### 2. Simple for Users, Powerful for Experts

**Simple API:**
```cpp
laya::log_info("Window created: {}x{}", width, height);
laya::log_warn("Texture load failed");
laya::log_error("Failed to initialize renderer");
```

**Advanced API:**
```cpp
laya::log(laya::log_category::video, laya::log_priority::debug, 
          "Frame buffer size: {}", size);

auto guard = laya::with_log_priority(laya::log_category::render, 
                                      laya::log_priority::verbose);
```

### 3. Type Safety

All SDL enums are wrapped in C++20 `enum class`:

```cpp
enum class log_priority {
    trace, verbose, debug, info, warn, error, critical
};

enum class log_category {
    application, error, assert_cat, system, audio, 
    video, render, input, test, gpu, custom
};
```

Conversion to SDL types happens internally and is never exposed.

---

## File Structure

```
include/laya/logging/
├── log_category.hpp    # Log category enum
├── log_priority.hpp    # Log priority enum
└── log.hpp            # Main logging API (templates + declarations)

src/laya/
└── log.cpp            # Implementation (SDL integration)
```

---

## Core Features

### Basic Logging

Seven priority levels matching SDL3:

```cpp
laya::log_trace("Very verbose debug info");
laya::log_verbose("Verbose debug info");
laya::log_debug("Debug message");
laya::log_info("Informational message");
laya::log_warn("Warning message");
laya::log_error("Error message");       // Auto includes file:line
laya::log_critical("Critical error");   // Auto includes file:line
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

This is implemented using C++20's `std::source_location`:

```cpp
template <class... Args>
void log_error(std::format_string<Args...> fmt, Args&&... args) {
    log_error(std::source_location::current(), fmt, 
              std::forward<Args>(args)...);
}
```

### Category-Specific Logging

Log to specific SDL categories:

```cpp
laya::log(laya::log_category::video, laya::log_priority::info, 
          "Video subsystem initialized");
          
laya::log(laya::log_category::audio, laya::log_priority::debug, 
          "Audio buffer size: {}", buffer_size);
```

### Priority Management

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

This follows laya's established pattern (see `renderer.hpp`'s state guards).

---

## Cross-Platform Colored Output

An optional QoL feature that colors log messages by severity:

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

### Implementation

Colors are implemented via a custom SDL log output function:

```cpp
void color_log_output(void* userdata, int category, 
                      SDL_LogPriority priority, const char* message) {
    const char* color_code = get_color_for_priority(priority);
    std::fprintf(stderr, "%s%s\033[0m\n", color_code, message);
}
```

This maintains SDL's atomicity guarantees for thread-safe logging.

---

## Implementation Details

### Template Design

All logging functions are templated to support `std::format`:

```cpp
template <class... Args>
inline void log_info(std::format_string<Args...> fmt, Args&&... args) {
    detail::log_message(log_category::application, log_priority::info,
                        std::format(fmt, std::forward<Args>(args)...).c_str());
}
```

**Why .c_str()?**
SDL3 expects C-style strings. We format to `std::string` then pass the C string to SDL. This is a conscious tradeoff - one extra allocation for safety and convenience.

### Internal Helpers

Conversion functions are in an anonymous namespace (not exposed):

```cpp
namespace {
    constexpr int to_sdl_category(log_category cat) noexcept;
    constexpr SDL_LogPriority to_sdl_priority(log_priority pri) noexcept;
    constexpr log_priority from_sdl_priority(SDL_LogPriority pri) noexcept;
}
```

These are `constexpr` for compile-time evaluation where possible.

### No SDL Exposure

SDL types never appear in public headers:

```cpp
// In public header (log.hpp):
void set_log_priority(log_category category, log_priority priority);

// In implementation (log.cpp):
void set_log_priority(log_category category, log_priority priority) {
    SDL_SetLogPriority(to_sdl_category(category), 
                       to_sdl_priority(priority));
}
```

Users never see `SDL_LogCategory` or `SDL_LogPriority`.

---

## Usage Examples

### Basic Application Logging

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

This is handled by SDL internally - we don't add overhead.

### Compile-Time Format Checking

`std::format_string<Args...>` provides compile-time verification:

```cpp
laya::log_info("Value: {}", 42);           // ✅ OK
laya::log_info("Value: {}", 42, "extra");  // ❌ Compile error
laya::log_info("Value: {:d}", "not int");  // ❌ Compile error
```

No runtime format string parsing or validation needed.

### Inline Templates

All templates are `inline` to enable compiler optimization:

```cpp
template <class... Args>
inline void log_info(std::format_string<Args...> fmt, Args&&... args) {
    // Compiles to direct SDL_LogMessage call with formatted string
}
```

---

## Comparison with SDL3 API

### Before (Raw SDL3):

```cpp
SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Window size: %dx%d", 800, 600);
SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Failed to create window: %s", SDL_GetError());
SDL_SetLogPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_VERBOSE);
```

### After (laya):

```cpp
laya::log_info("Window size: {}x{}", 800, 600);
laya::log_error("Failed to create window: {}", SDL_GetError());
laya::set_log_priority(laya::log_category::render, laya::log_priority::verbose);
```

**Benefits:**
- Type-safe formatting with `std::format`
- Compile-time format string checking
- No manual category specification for common use
- Consistent snake_case naming
- RAII for temporary state changes

---

## Testing

Unit tests cover:
- All log priority levels
- Format string support
- Category-specific logging
- Priority management
- RAII priority guards
- Color support functions
- Source location in error/critical messages

See `tests/unit/test_logging.cpp` for comprehensive test coverage.

---

## Future Considerations

Potential enhancements (if needed):

1. **Custom log handlers** - Ability to redirect logs to files, network, etc.
2. **Priority prefix customization** - Wrap `SDL_SetLogPriorityPrefix`
3. **Structured logging** - Add metadata beyond simple strings
4. **Log filtering** - Filter by message content, not just priority

However, staying true to the "thin wrapper" philosophy, these would only be added if there's clear user demand and they enhance SDL3's capabilities without adding unnecessary complexity.

---

## Summary

The laya logging system demonstrates the library's core values:

✅ **Thin wrapper** - Direct mapping to SDL3 with minimal overhead  
✅ **Type-safe** - Strong enums, compile-time format checking  
✅ **Modern C++** - `std::format`, `std::source_location`, RAII  
✅ **Zero SDL exposure** - All SDL types hidden in implementation  
✅ **Simple by default** - Easy to use for common cases  
✅ **Powerful when needed** - Advanced features for power users  
✅ **QoL improvements** - Colored output, automatic source location  

The implementation serves as a model for wrapping other SDL3 subsystems in the future.
