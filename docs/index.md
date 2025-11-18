# Laya Documentation

Welcome to the Laya documentation! Laya is a modern C++20 wrapper around SDL3, providing a type-safe, RAII-based interface for multimedia applications and games.

## What is Laya?

Laya wraps SDL3's C API in idiomatic C++20, offering:

- **Zero-Overhead Abstraction** - Compiles to identical assembly as hand-written SDL3
- **Memory Safety** - RAII for automatic resource management, eliminating leaks
- **Type Safety** - Strong types and compile-time checks prevent logical errors
- **Modern C++ Ergonomics** - Concepts, ranges, `std::format`, and more
- **Cross-Platform** - Identical behavior across all C++20 and SDL3 supported platforms

## Quick Example

```cpp
#include <laya/laya.hpp>

int main() {
    laya::context ctx{laya::subsystem::video};
    
    laya::window win{"My Game", {800, 600}, laya::window_flags::resizable};
    laya::renderer renderer{win};
    
    bool running = true;
    while (running) {
        for (const auto& event : laya::poll_events()) {
            if (std::holds_alternative<laya::quit_event>(event)) {
                running = false;
            }
        }
        
        renderer.clear(laya::color::black());
        // Drawing code here
        renderer.present();
    }
    
    return 0;
}
```

## Documentation Structure

### Getting Started
- **[Getting Started](getting-started.md)** - Installation, integration, and first steps
- **[Goals](goals.md)** - Project vision, design philosophy, and objectives
- **[Architecture](architecture.md)** - Design patterns and wrapping techniques

### Features
Detailed documentation for major Laya features:

- **[Windows](features/windows.md)** - Window creation and management
- **[Rendering](features/rendering.md)** - 2D rendering and drawing
- **[Events](features/events.md)** - Event handling and polling
- **[Logging](features/logging.md)** - Type-safe logging system

## Project Links

- **GitHub**: [github.com/radicazz/laya](https://github.com/radicazz/laya)
- **License**: MIT License
- **SDL3**: Built on top of [SDL3](https://github.com/libsdl-org/SDL)

## Key Features

### Memory Safety
```cpp
// Resources automatically cleaned up - no leaks possible
{
    laya::window win{"Title", {800, 600}};
    laya::renderer ren{win};
    // Automatic cleanup on scope exit
}
```

### Type Safety
```cpp
// Strong types prevent errors at compile time
win.set_size({800, 600});      // OK
win.set_position({100, 50});   // OK
// win.set_size({100, 50});    // Won't compile - different types!
```

### Modern Error Handling
```cpp
// Exceptions for unrecoverable errors
try {
    laya::window win{"Title", {800, 600}};
} catch (const laya::error& e) {
    laya::log_error("Window creation failed: {}", e.what());
}
```

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](../CONTRIBUTING.md) for guidelines.

## Community

For questions, issues, or discussions, please visit our [GitHub repository](https://github.com/radicazz/laya).
