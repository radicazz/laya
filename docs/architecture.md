# Architecture

This document outlines the C++20 design patterns, techniques, and architectural principles used to create safe, efficient wrappers around SDL3's C API.

______________________________________________________________________

## Core Principles

### 1. **RAII (Resource Acquisition Is Initialization)**

Every SDL resource is wrapped in a C++ class that automatically manages its lifetime.

```cpp
// Bad: Manual resource management
SDL_Window* window = SDL_CreateWindow("Title", 800, 600, 0);
// ... use window
SDL_DestroyWindow(window);  // Easy to forget or skip on error paths

// Good: RAII wrapper
class window {
    SDL_Window* m_window;
public:
    window(std::string_view title, size sz, window_flags flags = window_flags::none)
        : m_window(SDL_CreateWindow(title.data(), sz.width, sz.height,
                                     static_cast<uint32_t>(flags)))
    {
        if (!m_window) {
            throw error::from_sdl();
        }
    }

    ~window() {
        if (m_window) {
            SDL_DestroyWindow(m_window);
        }
    }

    // Non-copyable, movable
    window(const window&) = delete;
    window& operator=(const window&) = delete;

    window(window&& other) noexcept
        : m_window(std::exchange(other.m_window, nullptr)) {}

    window& operator=(window&& other) noexcept {
        if (this != &other) {
            if (m_window) SDL_DestroyWindow(m_window);
            m_window = std::exchange(other.m_window, nullptr);
        }
        return *this;
    }
};
```

### 2. **Strong Type Safety**

Replace SDL's loose typing with C++ strong types to prevent logical errors at compile time.

```cpp
// Bad: Easy to mix up parameters
SDL_SetWindowSize(window, 600, 800);  // Oops, swapped width/height

// Good: Strong types prevent errors
struct size { int width, height; };
struct position { int x, y; };

void window::set_size(size sz) {
    SDL_SetWindowSize(m_window, sz.width, sz.height);
}

void window::set_position(position pos) {
    SDL_SetWindowPosition(m_window, pos.x, pos.y);
}

// Usage - impossible to mix up
win.set_size({800, 600});
win.set_position({100, 50});
```

### 3. **Enum Class for Constants**

Replace SDL's C-style defines with type-safe enum classes.

```cpp
// Bad: C-style defines, no type safety
SDL_CreateWindow("Title", 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);

// Good: Type-safe enum class with bitwise operations
enum class window_flags : uint32_t {
    none = 0,
    fullscreen = SDL_WINDOW_FULLSCREEN,
    resizable = SDL_WINDOW_RESIZABLE,
    borderless = SDL_WINDOW_BORDERLESS,
    always_on_top = SDL_WINDOW_ALWAYS_ON_TOP,
};

// Enable bitwise operations
constexpr window_flags operator|(window_flags lhs, window_flags rhs) {
    return static_cast<window_flags>(
        static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
    );
}

// Usage
auto flags = window_flags::resizable | window_flags::borderless;
window win("Title", {800, 600}, flags);
```

______________________________________________________________________

## Error Handling

### Exception-Based Error Handling

Convert SDL's error codes into C++ exceptions for automatic error propagation.

```cpp
class error : public std::runtime_error {
public:
    explicit error(const std::string& msg) : std::runtime_error(msg) {}

    static error from_sdl() {
        const char* sdl_error = SDL_GetError();
        return error(sdl_error ? sdl_error : "Unknown SDL error");
    }
};

// Usage in constructors
window::window(std::string_view title, size sz, window_flags flags) {
    m_window = SDL_CreateWindow(title.data(), sz.width, sz.height,
                                 static_cast<uint32_t>(flags));
    if (!m_window) {
        throw error::from_sdl();
    }
}
```

______________________________________________________________________

## Modern C++20 Features

### 1. **Concepts for Type Constraints**

Use C++20 concepts to constrain template parameters and provide better error messages.

```cpp
#include <concepts>

template<typename T>
concept ColorLike = requires(T t) {
    { t.r } -> std::convertible_to<uint8_t>;
    { t.g } -> std::convertible_to<uint8_t>;
    { t.b } -> std::convertible_to<uint8_t>;
    { t.a } -> std::convertible_to<uint8_t>;
};

template<ColorLike Color>
void renderer::set_draw_color(const Color& c) {
    SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
}
```

### 2. **Constexpr for Compile-Time Computation**

Use `constexpr` to move computations to compile time when possible.

```cpp
struct color {
    uint8_t r, g, b, a;

    // Compile-time color creation
    static constexpr color rgb(uint8_t red, uint8_t green, uint8_t blue) {
        return {red, green, blue, 255};
    }

    // Compile-time color from hex
    static constexpr color from_hex(uint32_t hex) {
        return {
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF),
            255
        };
    }

    // Common colors as compile-time constants
    static constexpr color white() { return from_hex(0xFFFFFF); }
    static constexpr color black() { return from_hex(0x000000); }
    static constexpr color red() { return from_hex(0xFF0000); }
};

// Usage - computed at compile time
constexpr auto bg_color = color::from_hex(0x2E3440);
```

### 3. **std::format for Type-Safe Formatting**

Leverage `std::format` for compile-time checked string formatting in logging.

```cpp
template <class... Args>
inline void log_info(std::format_string<Args...> fmt, Args&&... args) {
    auto message = std::format(fmt, std::forward<Args>(args)...);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
}

// Usage - format checked at compile time
laya::log_info("Window size: {}x{}", width, height);
```

______________________________________________________________________

## Event System Design

### Type-Safe Event Variants

Use `std::variant` to create a type-safe event system.

```cpp
#include <variant>

// Individual event types
struct quit_event {
    uint32_t timestamp;
};

struct key_event {
    enum class state { pressed, released };
    uint32_t timestamp;
    state key_state;
    // ... key details
};

struct mouse_button_event {
    enum class state { pressed, released };
    enum class button { left = 1, middle = 2, right = 3 };
    uint32_t timestamp;
    state button_state;
    button mouse_button;
    int x, y;
};

// Event variant
using event = std::variant<
    quit_event,
    key_event,
    mouse_button_event
    // ... other event types
>;
```

### Range-Based Event Polling

Provide a range-based interface for event polling.

```cpp
class event_range {
    std::vector<event> m_events;

public:
    event_range() {
        SDL_Event sdl_event;
        while (SDL_PollEvent(&sdl_event)) {
            m_events.push_back(from_sdl_event(sdl_event));
        }
    }

    auto begin() const { return m_events.begin(); }
    auto end() const { return m_events.end(); }
    bool empty() const { return m_events.empty(); }
};

// Usage
for (const auto& ev : laya::poll_events()) {
    std::visit([](const auto& event) {
        using T = std::decay_t<decltype(event)>;
        if constexpr (std::is_same_v<T, quit_event>) {
            // Handle quit
        } else if constexpr (std::is_same_v<T, key_event>) {
            // Handle key
        }
    }, ev);
}
```

______________________________________________________________________

## Efficient Abstractions

### 1. **Inline Functions for Simple Wrappers**

Mark simple wrapper functions as `inline` to minimize overhead.

```cpp
class window {
    SDL_Window* m_window;

public:
    inline void show() { SDL_ShowWindow(m_window); }
    inline void hide() { SDL_HideWindow(m_window); }
    inline void maximize() { SDL_MaximizeWindow(m_window); }
    inline void minimize() { SDL_MinimizeWindow(m_window); }

    inline size size() const {
        int w, h;
        SDL_GetWindowSize(m_window, &w, &h);
        return {w, h};
    }
};
```

### 2. **Compile-Time Tests**

Use `static_assert` to validate assumptions at compile time.

```cpp
// Ensure our wrappers have the same size as SDL types
static_assert(sizeof(laya::color) == 4, "Color should be 4 bytes");
static_assert(sizeof(laya::point) == 8, "Point should be 8 bytes");
static_assert(sizeof(laya::rect) == 16, "Rect should be 16 bytes");

// Ensure enum values match SDL constants
static_assert(static_cast<uint32_t>(window_flags::resizable)
              == SDL_WINDOW_RESIZABLE);
```

______________________________________________________________________

## Memory Management

### RAII with Move Semantics

All resource-owning types are non-copyable but movable.

```cpp
class window {
    SDL_Window* m_window;

public:
    // Non-copyable
    window(const window&) = delete;
    window& operator=(const window&) = delete;

    // Movable
    window(window&& other) noexcept
        : m_window(std::exchange(other.m_window, nullptr)) {}

    window& operator=(window&& other) noexcept {
        if (this != &other) {
            if (m_window) SDL_DestroyWindow(m_window);
            m_window = std::exchange(other.m_window, nullptr);
        }
        return *this;
    }

    ~window() {
        if (m_window) {
            SDL_DestroyWindow(m_window);
        }
    }
};
```

______________________________________________________________________

## Design Patterns

### 1. **Thin Wrapper Philosophy**

Laya provides thin wrappers that map directly to SDL3 functions:

- Direct mapping to SDL3 API
- Minimal overhead - just type safety and convenience
- No additional abstractions beyond what SDL provides
- Optional QoL features don't interfere with core functionality

### 2. **No SDL Exposure in Public Headers**

SDL types never appear in public headers:

```cpp
// In public header (window.hpp):
void set_title(std::string_view title);

// In implementation (window.cpp):
void window::set_title(std::string_view title) {
    SDL_SetWindowTitle(m_window, title.data());
}
```

Users never see `SDL_Window*` or other SDL types directly.

### 3. **Type Conversion Helpers**

Conversion functions are internal and not exposed:

```cpp
namespace {  // anonymous namespace - internal only
    constexpr int to_sdl_category(log_category cat) noexcept {
        return static_cast<int>(cat);
    }

    constexpr SDL_LogPriority to_sdl_priority(log_priority pri) noexcept {
        return static_cast<SDL_LogPriority>(pri);
    }
}
```

______________________________________________________________________

## File Organization

```
include/laya/
├── laya.hpp              # Main include file
├── errors.hpp            # Error handling types
├── subsystems.hpp        # Subsystem initialization
├── bitmask.hpp           # Bitmask utilities
├── events/
│   ├── event_types.hpp   # Event variant types
│   ├── event_polling.hpp # Event polling API
│   └── event_window.hpp  # Window events
├── windows/
│   ├── window.hpp        # Window class
│   ├── window_flags.hpp  # Window flags enum
│   └── window_id.hpp     # Window ID type
├── renderers/
│   ├── renderer.hpp      # Renderer class
│   ├── renderer_flags.hpp
│   └── renderer_types.hpp
└── logging/
    ├── log.hpp           # Logging API
    ├── log_priority.hpp  # Priority enum
    └── log_category.hpp  # Category enum

src/laya/
├── window.cpp            # Window implementation
├── renderer.cpp          # Renderer implementation
├── log.cpp              # Logging implementation
├── event_polling.cpp    # Event polling implementation
└── subsystems.cpp       # Subsystem initialization
```

______________________________________________________________________

## Testing Strategy

### 1. **Unit Tests**

Test individual components in isolation:

```cpp
TEST_CASE("Window RAII") {
    laya::context ctx{laya::subsystem::video};

    {
        laya::window win{"Test", {800, 600}};
        CHECK(win.native_handle() != nullptr);
    }
    // Window automatically destroyed
}
```

### 2. **Performance Benchmarks**

Measure overhead and optimize hot paths:

```cpp
void benchmark_laya_wrapper() {
    laya::renderer ren{window};
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; ++i) {
        ren.set_draw_color(laya::color::red());
    }

    auto end = std::chrono::high_resolution_clock::now();
    // Compare with raw SDL to measure wrapper overhead
}
```

### 3. **Integration Tests**

Test real-world usage patterns in the `examples/` directory.

______________________________________________________________________

## Summary

Laya's architecture demonstrates these key principles:

✅ **RAII** - Automatic resource management
✅ **Type Safety** - Strong enums, compile-time checks
✅ **Modern C++20** - Concepts, `std::format`, constexpr
✅ **Minimal Overhead** - Inline functions, efficient design
✅ **Thin Wrapper** - Direct mapping to SDL3
✅ **No SDL Exposure** - Clean public API

This architecture serves as the foundation for all Laya components, ensuring consistency, safety, and performance across the entire library.
