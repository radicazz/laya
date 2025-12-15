# Getting Started

Quick path to your first window with laya.

## Prerequisites

- C++20 compiler (GCC 11+, Clang 14+, MSVC 2022+)
- CMake 3.21 or newer

## Add to your project

Recommended: CMake FetchContent handles SDL3 for you.

```cmake
include(FetchContent)
FetchContent_Declare(
  laya
  GIT_REPOSITORY https://github.com/radicazz/laya.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(laya)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE laya::laya)
```

Prefer a submodule? Drop the repo at `external/laya` and `add_subdirectory(external/laya)`.

## Build and run

```bash
cmake -B build -S .
cmake --build build
./build/my_app
```

## Minimal program

```cpp
#include <laya/laya.hpp>

int main() {
    laya::context ctx{laya::subsystem::video};
    laya::window win{"Quick start", {800, 600}};
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

## Common tweaks

- Set `LAYA_BUILD_ALL OFF` when consuming Laya to skip tests/examples.
- Already have SDL3 targets? Set `LAYA_SDL_TARGETS_PROVIDED ON` before `add_subdirectory`.

## Stuck?

- First configure takes longer while SDL3 downloads—only once.
- If `SDL3::SDL3` is missing, ensure internet access or install SDL3 system-wide.
