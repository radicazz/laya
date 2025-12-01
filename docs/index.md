# Laya Documentation

Welcome to the Laya documentation! Laya is a modern C++20 wrapper around SDL3, providing a type-safe, RAII-based interface for multimedia applications and games.

## What is Laya?

Laya wraps SDL3's C API in idiomatic C++20, offering:

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

---

## Getting Started

### Prerequisites

- **C++20 compatible compiler** (GCC 11+, Clang 14+, MSVC 2022+)
- **CMake 3.21 or higher**
- **SDL3** (automatically handled by Laya)

### Installation

#### Option 1: FetchContent (Recommended)

The easiest way to add Laya to your project. SDL3 dependencies are managed automatically!

```cmake
include(FetchContent)
FetchContent_Declare(
    laya
    GIT_REPOSITORY https://github.com/radicazz/laya.git
    GIT_TAG        main  # or specific version tag
)
FetchContent_MakeAvailable(laya)
target_link_libraries(your_app PRIVATE laya::laya)
```

**Note:** No manual dependency setup required! Laya will automatically download and configure SDL3 on first build.

#### Option 2: Git Submodule

Add the repository as a git submodule:

```bash
cd your_project_root

# Add laya as a submodule
git submodule add https://github.com/radicazz/laya.git external/laya
git submodule update --init  # Only initializes doctest for testing
```

Then, update your `CMakeLists.txt`:

```cmake
# CMakeLists.txt
add_subdirectory(external/laya)
target_link_libraries(your_app PRIVATE laya::laya)
```

#### Option 3: CPM (CMake Package Manager)

```cmake
include(cmake/CPM.cmake)
CPMAddPackage("gh:radicazz/laya@main")
target_link_libraries(your_app PRIVATE laya::laya)
```

### Your First Laya Application

Create a simple window application:

```cpp
// main.cpp
#include <laya/laya.hpp>

int main() {
    // Initialize SDL subsystems
    laya::context ctx{laya::subsystem::video};

    // Create a window
    laya::window win{"My First Window", {800, 600}};

    // Keep window open until user closes it
    bool running = true;
    while (running) {
        for (const auto& event : laya::poll_events()) {
            if (std::holds_alternative<laya::quit_event>(event)) {
                running = false;
            }
        }
    }

    return 0;
}
```

#### CMake Configuration

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(my_app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Laya
add_subdirectory(external/laya)

# Create executable
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE laya::laya)
```

#### Build and Run

```bash
# Configure
cmake -B build -S .

# Build
cmake --build build

# Run
./build/my_app
```

### Complete Example with Rendering

```cpp
#include <laya/laya.hpp>

int main() {
    try {
        // Initialize video subsystem
        laya::context ctx{laya::subsystem::video};
        laya::log_info("Laya initialized successfully");

        // Create window
        laya::window win{"Laya Example", {800, 600},
                         laya::window_flags::resizable};
        laya::log_info("Window created");

        // Create renderer
        laya::renderer renderer{win};
        laya::log_info("Renderer created");

        // Main loop
        bool running = true;
        while (running) {
            // Handle events
            for (const auto& event : laya::poll_events()) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    running = false;
                }
            }

            // Clear screen with blue color
            renderer.clear(laya::color{100, 149, 237});

            // Draw a red rectangle
            renderer.set_draw_color(laya::color{255, 0, 0});
            renderer.fill_rect({100, 100, 200, 150});

            // Present to screen
            renderer.present();
        }

        laya::log_info("Application exiting");

    } catch (const laya::error& e) {
        laya::log_critical("Fatal error: {}", e.what());
        return 1;
    }

    return 0;
}
```

---

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

---

## Documentation Structure

### Core Documentation
- **[Goals](goals.md)** - Project vision, design philosophy, and objectives
- **[Architecture](architecture.md)** - Design patterns and wrapping techniques

### Features
Detailed documentation for major Laya features:

- **[Windows](features/windows.md)** - Window creation and management
- **[Rendering](features/rendering.md)** - 2D rendering and drawing
- **[Events](features/events.md)** - Event handling and polling
- **[Logging](features/logging.md)** - Type-safe logging system

---

## Project Links

- **GitHub**: [github.com/radicazz/laya](https://github.com/radicazz/laya)
- **License**: MIT License
- **SDL3**: Built on top of [SDL3](https://github.com/libsdl-org/SDL)

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](https://github.com/radicazz/laya/blob/master/CONTRIBUTING.md) for guidelines.

## Community

For questions, issues, or discussions, please visit our [GitHub repository](https://github.com/radicazz/laya).
