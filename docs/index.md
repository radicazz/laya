# in a nutshell

Laya is a modern C++20 wrapper around SDL3 that keeps the low-level power but adds safety rails and friendlier ergonomics.

## Why laya exists

- SDL3 is great at cross‑platform multimedia, but its C API makes it easy to leak, crash, or mix up parameters.
- Laya wraps that API with RAII, strong types, and modern C++ features so you spend time shipping features—not chasing segfaults.
- The goal is a thin layer: zero‑surprise, zero‑overhead, still letting you drop to SDL when you need to.

## What you get

- Automatic cleanup for windows, renderers, textures, and more (RAII all the way).
- Strongly‑typed flags, sizes, positions, and colors that catch mistakes at compile time.
- Range-based event polling with `std::variant` events.
- Built-in logging that uses `std::format`.
- Cross-platform defaults and an SDL dependency that can fetch itself when you build.

## 60-second start

1. Add Laya with CMake FetchContent:

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

1. Configure and build:

   ```bash
   cmake -B build -S .
   cmake --build build
   ```

## Tiny example

```cpp
#include <laya/laya.hpp>

int main() {
    laya::context ctx{laya::subsystem::video};
    laya::window win{"Hello laya", {800, 600}};
    laya::renderer ren{win};

    for (bool running = true; running; ) {
        for (auto& ev : laya::poll_events()) {
            if (std::holds_alternative<laya::quit_event>(ev)) running = false;
        }
        ren.clear(laya::color::black());
        ren.present();
    }
}
```

## Next steps

- **Getting Started** for a quick setup walkthrough.
- **Key Features** to see what Laya covers today.
- Dive deeper in the repo when you need implementation details.
