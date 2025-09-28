# Quick Start

Laya is a C++20 game framework that simplifies game development by providing a high-level API built on top of SDL3. This guide will help you integrate Laya into your CMake-based project.

## Option 1: Git Submodule (Recommended)

Add the repositiory as a git submodule:

```bash
# Assuming your project uses `external/` for dependencies
git submodule add https://github.com/radicazz/laya.git external/laya
git submodule update --init --recursive
```

Then, update your `CMakeLists.txt` to include Laya:

```cmake
# CMakeLists.txt
add_subdirectory(external/laya)
target_link_libraries(your_app PRIVATE laya::laya)
```

## Option 2: FetchContent

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
| `LAYA_SDL_METHOD` | "submodule" | How to consume SDL ("submodule" or "system") |
| `LAYA_USE_SDL_IMAGE` | ON | Enable SDL_image support |
| `LAYA_USE_SDL_TTF` | ON | Enable SDL_ttf support |
| `LAYA_SDL_TARGETS_PROVIDED` | OFF | Skip SDL setup (parent provides targets) |

## Usage Examples

### Minimal Integration

```cmake
# Just add Laya, no configuration needed
add_subdirectory(external/laya)
target_link_libraries(my_game PRIVATE laya::laya)
```

### Custom Configuration

```cmake
# Configure before adding subdirectory
set(LAYA_USE_SDL_IMAGE OFF)    # Disable image support
set(LAYA_USE_SDL_TTF ON)       # Keep TTF support
set(LAYA_SDL_METHOD "system")  # Use system SDL instead of submodules

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

Laya handles SDL dependencies automatically based on your configuration:

### Submodule Method (Default)

- Uses git submodules in `external/` directory
- Builds SDL as shared libraries
- Automatically copies DLLs on Windows
- **Pros**: Consistent versions, no system dependencies
- **Cons**: Larger repository, longer build times

### System Method

- Uses system-installed SDL libraries
- Requires SDL3, SDL3_image, SDL3_ttf to be installed
- **Pros**: Faster builds, smaller repo
- **Cons**: Version compatibility responsibility

```cmake
# Use system SDL
set(LAYA_SDL_METHOD "system")
add_subdirectory(external/laya)
```

## Best Practices

### For Application Developers

```cmake
# Minimal, production-ready setup
set(LAYA_BUILD_ALL OFF)           # No tests/examples
set(LAYA_SDL_METHOD "submodule")  # Consistent dependencies
add_subdirectory(external/laya)
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
set(LAYA_SDL_METHOD "submodule")
add_subdirectory(.)  # Building Laya as root project
```

## Troubleshooting

### Missing SDL Submodules

```bash
git submodule update --init --recursive
```

### Build Errors with System SDL

Ensure SDL3 development packages are installed:

```bash
# Ubuntu/Debian
sudo apt install libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev

# Windows
vcpkg install sdl3 sdl3-image sdl3-ttf
```

### Target Not Found Errors

If you see `SDL3::SDL3 target not found`:

- Check `LAYA_SDL_TARGETS_PROVIDED` is correctly set
- Verify your SDL targets are created before `add_subdirectory(laya)`

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
