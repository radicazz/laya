<!-- Temporary disable markdownlint for specific rules to allow inline html -->
<!-- markdownlint-disable MD033 MD041 -->

<div align="center">
  <image alt="logo" src="docs/assets/logo_cropped.png" width="300"/>
</div>

<div align="center">
    <p>
        <a href="docs/quick_start.md">Getting Started</a> •
        <a href="examples">Usage Examples</a> •
        <a href="https://github.com/radicazz/laya/releases">Releases</a> •
        <a href="docs/design/roadmap.md">Roadmap</a>
    </p>
</div>

<div align="center">
    <p>An intuitive cross-platform library for seamless, type-safe and efficient multi-media app development with C++.</p>
</div>

<div align="center">
    <image alt="GitHub Last Commit" src="https://img.shields.io/github/last-commit/radicazz/laya?logo=github&logoColor=white"/>
    <image alt="Windows Build Status" src="https://github.com/radicazz/laya/actions/workflows/build_windows.yml/badge.svg?branch="/>
    <image alt="Linux Build Status" src="https://github.com/radicazz/laya/actions/workflows/build_linux.yml/badge.svg?branch="/>
</div>

<div align="center">
    <image alt="C++ 20 Badge" src="https://img.shields.io/badge/C%2B%2B-20-064F8C?logo=cplusplus&logoColor=white"/>
    <image alt="SDL3 Badge" src="https://img.shields.io/badge/SDL-3-064F8C?logo=files&logoColor=white"/>
    <image alt="CMake Badge" src="https://img.shields.io/badge/CMake-3.21-064F8C?logo=cmake&logoColor=white"/>
</div>

<div align="center">
    <image alt="License: MIT" src="https://img.shields.io/github/license/radicazz/laya?color=orange&logo=open-source-initiative&logoColor=white"/>
</div>

<!--
<div align="center">
    <p>
        <a href="docs/quick_start.md">Quick Start</a> •
        <a href="examples">Examples</a> •
        <a href="https://github.com/radicazz/laya/releases">Releases</a> •
        <a href="docs/design/roadmap.md">Roadmap</a>
    </p>
</div>
-->

<!--
<div align="center">
    <image alt="GitHub Repo Size" src="https://img.shields.io/github/repo-size/radicazz/laya?logo=github&logoColor=white"/>
    <image alt="License: MIT" src="https://img.shields.io/github/license/radicazz/laya?color=orange&logo=open-source-initiative&logoColor=white"/>
    <image alt="GitHub Issues" src="https://img.shields.io/github/issues/radicazz/laya?logo=github&logoColor=white"/>
</div>
-->

<!--
<div align="center">
    <p>
    The idea behind <i>laya</i> is to provide a clean, type-safe and idiomatic C++ interface for SDL and its extensions that feels natural while preserving the full power and flexibility of the underlying C library.
    </p>
</div>
-->

<!-- markdownlint-enable MD033 MD041 -->

# Overview

A modern library targetting C++20 and SDL3 for cross-platform, immediate-mode, desktop application development. With *laya*, you can create windows, handle input events, render 2D graphics and manage resources in a type-safe and efficient manner while leveraging the full power of the underlying SDL library.

```cpp
#include <laya/laya.hpp>

int main() {
    // Initialize SDL and free when out of scope
    laya::context ctx(laya::subsystem::video);

    laya::window window("Hello, Laya!", {800, 600});
    laya::renderer renderer(window);

    bool running = true;

    while (running) {
        for (const auto& event : laya::events_view()) {
            // Destroy the window when requested
            if (std::holds_alternative<laya::quit_event>(event)) {
                running = false;
                break;
            }
        }

        renderer.set_draw_color(laya::colors::black);
        renderer.clear();

        // Draw a rectangle
        renderer.set_draw_color(laya::colors::white);
        renderer.fill_rect({100, 100, 200, 150});

        renderer.present();
    }
}
```

## Installation

Including *laya* in your project is easy. Laya automatically manages SDL3 dependencies using CMake FetchContent - no manual setup required!

### Option 1: FetchContent (Recommended)

```cmake
include(FetchContent)

FetchContent_Declare(
    laya
    GIT_REPOSITORY https://github.com/radicazz/laya.git
    GIT_TAG main  # or specify a release tag
)
FetchContent_MakeAvailable(laya)

target_link_libraries(your_app PRIVATE laya::laya)
```

### Option 2: Git Submodule

```bash
cd your_project_root

# Add laya as a submodule
git submodule add https://github.com/radicazz/laya.git external/laya
```

Then, update your `CMakeLists.txt`:

```cmake
# CMakeLists.txt
add_subdirectory(external/laya)
target_link_libraries(your_app PRIVATE laya::laya)
```

### Advanced Configuration

- If you already have SDL3 in your project, laya will use it automatically
- Use system-installed SDL3: It will be detected automatically via `find_package`

See [Quick Start](docs/quick_start.md) for more details.

# Contributing

If you are interested in contributing, please see [CONTRIBUTING.md](CONTRIBUTING.md) for some guidelines and best practices.

# License

This project falls under the MIT License. See [LICENSE.txt](LICENSE.txt) for details.
