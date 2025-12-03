# Getting Started

Welcome to the getting started guide for integrating *laya* into your C++ project. This guide covers installation, configuration, and your first *laya* application.

## Prerequisites

- **C++20 compatible compiler** (GCC 11+, Clang 14+, MSVC 2022+)
- **CMake 3.21 or higher**
- **SDL3** (automatically handled by *laya*)

## Installation

### Option 1: FetchContent (Recommended)

The easiest way to add *laya* to your project. SDL3 dependencies are managed automatically!

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

**Note:** No manual dependency setup required! *laya* will automatically download and configure SDL3 on first build.

### Option 2: Git Submodule

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

### Option 3: CPM (CMake Package Manager)

```cmake
include(cmake/CPM.cmake)
CPMAddPackage("gh:radicazz/laya@main")
target_link_libraries(your_app PRIVATE laya::laya)
```

## Configuration Options

### Basic Options

| Option | Default | Description |
|--------|---------|-------------|
| `LAYA_BUILD_ALL` | OFF when consumed | Build tests and examples |
| `LAYA_BUILD_TESTS` | Follows LAYA_BUILD_ALL | Build test suite |
| `LAYA_BUILD_EXAMPLES` | Follows LAYA_BUILD_ALL | Build example programs |

### SDL Dependency Options

| Option | Default | Description |
|--------|---------|-------------|
| `LAYA_SDL_TARGETS_PROVIDED` | OFF | Skip SDL setup (parent provides SDL3) |

## SDL Dependency Handling

*laya* uses an intelligent fallback chain to manage SDL3 dependencies automatically:

### Automatic Resolution (Default)

1. **Parent-provided targets**: If your project already has SDL3 targets, they're used automatically
2. **System SDL3**: *laya* tries `find_package(SDL3)` first
3. **FetchContent**: If not found, SDL3 is downloaded and built automatically

**Benefits:**
- ✅ Zero configuration in most cases
- ✅ Works with existing SDL3 installations
- ✅ No git submodules needed for SDL
- ✅ Cached downloads (fast rebuilds)
- ✅ Cross-platform consistency

### Using System SDL3

If you have SDL3 installed system-wide, *laya* will detect and use it automatically via `find_package()`:

```bash
# Ubuntu/Debian
sudo apt install libsdl3-dev

# macOS
brew install sdl3

# Windows (vcpkg)
vcpkg install sdl3
```

No CMake configuration needed - it just works!

### Using Your Own SDL3

If your project already provides SDL3 targets:

```cmake
# Your project provides SDL3::SDL3 target
find_package(SDL3 REQUIRED)  # or FetchContent, etc.

# Tell *laya* to use your targets
set(LAYA_SDL_TARGETS_PROVIDED ON)
add_subdirectory(external/laya)
```

## Your First *laya* Application

### Minimal Example

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

### CMake Configuration

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(my_app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add *laya*
add_subdirectory(external/laya)

# Create executable
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE laya::laya)
```

### Build and Run

```bash
# Configure
cmake -B build -S .

# Build
cmake --build build

# Run
./build/my_app
```

## Complete Example with Rendering

```cpp
#include <laya/laya.hpp>

int main() {
    try {
        // Initialize video subsystem
        laya::context ctx{laya::subsystem::video};
        laya::log_info("*laya* initialized successfully");

        // Create window
        laya::window win{"*laya* Example", {800, 600},
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

## Best Practices

### For Application Developers

```cmake
# Minimal, production-ready setup
set(LAYA_BUILD_ALL OFF)  # No tests/examples needed
add_subdirectory(external/laya)
# SDL3 is handled automatically via FetchContent
```

### For Library Developers

```cmake
# When *laya* is a dependency of your library
set(LAYA_BUILD_ALL OFF)
set(LAYA_SDL_TARGETS_PROVIDED ON)  # You provide SDL
add_subdirectory(external/laya)
```

### For *laya* Contributors

```cmake
# Development with all features
set(LAYA_BUILD_ALL ON)
add_subdirectory(.)  # Building *laya* as root project
# SDL3 is fetched automatically on first build
```

## Troubleshooting

### First Build Takes Longer

On first configure, CMake downloads SDL3 using FetchContent. This is normal and only happens once - the download is cached.

### Internet Connection Issues

If you're behind a proxy or firewall:

```cmake
# Option 1: Install SDL3 system-wide (it will be auto-detected)
# Option 2: Pre-populate your CMake cache with SDL3
```

### Target Not Found Errors

If you see `SDL3::SDL3 target not found`:

- Check `LAYA_SDL_TARGETS_PROVIDED` is correctly set
- Verify your SDL targets are created before `add_subdirectory(laya)`
- Ensure internet connection is available for FetchContent

### Using Offline Builds

If you need to build without internet:

1. Build once with internet (populates CMake's FetchContent cache)
2. OR install SDL3 system-wide, which will be detected automatically

## Platform-Specific Notes

### Linux

```bash
# Install build dependencies
sudo apt install build-essential cmake git

# Optional: Install SDL3 system-wide
sudo apt install libsdl3-dev
```

### macOS

```bash
# Install via Homebrew
brew install cmake

# Optional: Install SDL3 system-wide
brew install sdl3
```

### Windows

Use Visual Studio 2022 or later with CMake support, or install CMake separately.

PowerShell:
```powershell
# Configure with Visual Studio generator
cmake -B build -G "Visual Studio 17 2022" -A x64

# Build
cmake --build build --config Release
```

## Next Steps

- **[Goals](goals.md)** - Understand *laya*'s design philosophy
- **[Architecture](architecture.md)** - Learn about *laya*'s design patterns
- **[Features](features/logging.md)** - Explore specific features like logging, events, and rendering
- **[Examples](../examples/)** - See complete example applications

## Getting Help

- **Issues**: [GitHub Issues](https://github.com/radicazz/laya/issues)
- **Documentation**: This documentation
- **Examples**: Check the `examples/` directory in the repository

Happy coding with *laya*!
