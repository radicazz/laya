# Quick Start

Welcome to the quick start guide for integrating the Laya library into your C++ project. This will cover the basics of adding Laya as a dependency, configuring it, and using it in your application.

## Option 1: FetchContent (Recommended)

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

## Option 2: Git Submodule

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

## Option 3: CPM (CMake Package Manager)

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

## Usage Examples

### Minimal Integration

```cmake
# Just add Laya, no configuration needed
add_subdirectory(external/laya)
target_link_libraries(my_game PRIVATE laya::laya)
```

### Custom Configuration

```cmake
# Use parent's SDL3 instead of auto-download
find_package(SDL3 REQUIRED)
set(LAYA_SDL_TARGETS_PROVIDED ON)
add_subdirectory(external/laya)
target_link_libraries(my_game PRIVATE laya::laya)
```

### When You Already Have SDL

```cmake
# Your project already provides SDL targets
find_package(SDL3 REQUIRED)
# ... other SDL setup ...

# Tell Laya to use your targets
set(LAYA_SDL_TARGETS_PROVIDED ON)
add_subdirectory(external/laya)
target_link_libraries(my_game PRIVATE laya::laya)
```

### Enabling Development Tools

```cmake
# Only enable when developing/debugging Laya itself
set(LAYA_BUILD_ALL ON)  # Enables tests and examples
add_subdirectory(external/laya)
```

## SDL Dependency Handling

Laya uses an intelligent fallback chain to manage SDL3 dependencies automatically:

### Automatic Resolution (Default)

1. **Parent-provided targets**: If your project already has SDL3 targets, they're used automatically
2. **System SDL3**: Laya tries `find_package(SDL3)` first
3. **FetchContent**: If not found, SDL3 is downloaded and built automatically

**Benefits:**
- ✅ Zero configuration in most cases
- ✅ Works with existing SDL3 installations
- ✅ No git submodules needed for SDL
- ✅ Cached downloads (fast rebuilds)
- ✅ Cross-platform consistency

### Using System SDL3

If you have SDL3 installed system-wide, Laya will detect and use it automatically via `find_package()`:

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

# Tell Laya to use your targets
set(LAYA_SDL_TARGETS_PROVIDED ON)
add_subdirectory(external/laya)
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
# When Laya is a dependency of your library
set(LAYA_BUILD_ALL OFF)
set(LAYA_SDL_TARGETS_PROVIDED ON)  # You provide SDL
add_subdirectory(external/laya)
```

### For Laya Contributors

```cmake
# Development with all features
set(LAYA_BUILD_ALL ON)
add_subdirectory(.)  # Building Laya as root project
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

## Minimal Example

```cpp
// main.cpp
#include <laya/laya.hpp>

int main() {
    laya::init();
    // Your game code here
    laya::quit();
    return 0;
}
```

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(my_game)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/laya)
add_executable(my_game main.cpp)
target_link_libraries(my_game PRIVATE laya::laya)
```

This setup provides a static Laya library with all SDL dependencies handled automatically.
