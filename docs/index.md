# *laya* Documentation

A modern C++20 wrapper around SDL3 that makes multimedia programming safer and more pleasant.

## What is *laya*?

Think of it as SDL3 with the safety rails up. *laya* wraps the C API in idiomatic C++20:

- **Memory Safety** - Resources clean themselves up automatically
- **Type Safety** - Catch bugs at compile time instead of runtime
- **Modern C++** - Concepts, ranges, `std::format` and friends
- **Cross-Platform** - Works everywhere SDL3 and C++20 work

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

## Goals

We want to give you SDL3 with modern C++ niceties and zero runtime cost:

- **Safety First** - RAII resource management and strong typing catch bugs at compile time
- **Thin Wrappers** - Direct mapping to SDL3 functions with minimal overhead
- **Modern C++20** - Concepts, ranges, `std::format`, `std::variant` events
- **Complete Coverage** - Wrap all of SDL3 so you never need raw C calls

**Philosophy**: Fail fast at compile time. Pay only for what you use. Be explicit over clever.

---

## Getting Started

### What You Need

- C++20 compiler (GCC 11+, Clang 14+, MSVC 2022+)
- CMake 3.21+
- SDL3 (we handle this for you)

### Installation

Add to your CMakeLists.txt:

```cmake
include(FetchContent)
FetchContent_Declare(
    laya
    GIT_REPOSITORY https://github.com/radicazz/laya.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(laya)
target_link_libraries(your_app PRIVATE laya::laya)
```

SDL3 downloads automatically on first build. No extra setup needed.

### Your First Window

```cpp
#include <laya/laya.hpp>

int main() {
    laya::context ctx{laya::subsystem::video};
    laya::window win{"Hello laya", {800, 600}};

    bool running = true;
    while (running) {
        for (const auto& event : laya::poll_events()) {
            if (std::holds_alternative<laya::quit_event>(event)) {
                running = false;
            }
        }
    }
}
```

Build it:

```bash
cmake -B build
cmake --build build
./build/my_app
```

### Drawing Something

```cpp
#include <laya/laya.hpp>

int main() {
    laya::context ctx{laya::subsystem::video};
    laya::window win{"*laya* Example", {800, 600}};
    laya::renderer renderer{win};

    bool running = true;
    while (running) {
        for (const auto& event : laya::poll_events()) {
            if (std::holds_alternative<laya::quit_event>(event)) {
                running = false;
            }
        }

        renderer.clear(laya::color{100, 149, 237});
        renderer.set_draw_color(laya::color{255, 0, 0});
        renderer.fill_rect({100, 100, 200, 150});
        renderer.present();
    }
}
```

---

## Why Use *laya*?

Resources clean themselves up:
```cpp
{
    laya::window win{"Title", {800, 600}};
    laya::renderer ren{win};
} // Everything freed automatically
```

Strong types catch mistakes:
```cpp
win.set_size({800, 600});      // OK
win.set_position({100, 50});   // OK
// win.set_size({100, 50});    // Won't compile!
```

---

## Learn More

- **[Windows](features/windows.md)** - Creating and managing windows
- **[Rendering](features/rendering.md)** - Drawing stuff on screen
- **[Events](features/events.md)** - Handling user input
- **[Logging](features/logging.md)** - Debugging and diagnostics

---

## Links

- [GitHub](https://github.com/radicazz/laya)
- [SDL3](https://github.com/libsdl-org/SDL)
- [Contributing](https://github.com/radicazz/laya/blob/master/CONTRIBUTING.md)
