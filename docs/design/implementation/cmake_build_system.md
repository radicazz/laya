# CMake Build System Implementation

## Overview

Laya uses modern CMake 3.21+ with FetchContent for automatic dependency management. SDL3 and its extensions are downloaded automatically on first configure, eliminating the need for git submodules.

## Migration Note (v0.1.0)

**Previous approach (deprecated):** Git submodules for SDL3  
**Current approach:** FetchContent with intelligent fallback chain

**Scope change:** SDL_image and SDL_ttf support removed - focusing on core SDL3 only

The new system provides:
- ✅ Zero manual setup - no `--recursive` cloning needed
- ✅ Automatic SDL3 download via FetchContent
- ✅ Smart fallback: parent targets → system SDL3 → FetchContent
- ✅ Works seamlessly whether SDL3 is pre-installed or not
- ✅ Cached downloads for fast rebuilds

## Dependency Resolution Strategy

Laya uses a three-tier fallback chain for SDL3 dependencies:

### 1. Parent-Provided Targets (Highest Priority)

If the parent project already provides SDL3 targets, Laya uses them:

```cmake
# Parent project provides SDL3
find_package(SDL3 REQUIRED)

# Tell Laya to use parent's SDL3
set(LAYA_SDL_TARGETS_PROVIDED ON)
add_subdirectory(external/laya)
```

### 2. System-Installed SDL3

Laya automatically tries `find_package(SDL3)`. If found, it uses the system installation:

```bash
# Ubuntu/Debian
sudo apt install libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev

# macOS
brew install sdl3 sdl3_image sdl3_ttf

# Windows (vcpkg)
vcpkg install sdl3 sdl3-image sdl3-ttf
```

### 3. FetchContent (Fallback)

If SDL3 is not found via the above methods, Laya automatically downloads and builds it using CMake's FetchContent:

```cmake
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.6
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL3)
```

## Directory Structure

```
laya/
├── CMakeLists.txt              # Root CMake configuration
├── cmake/
│   ├── SetupSDL3.cmake         # SDL3 FetchContent setup
│   ├── LayaConfig.cmake.in     # Package config template
│   └── LayaInstall.cmake       # Installation configuration
├── external/
│   └── doctest/                # Test framework (git submodule)
├── include/
│   └── laya/                   # Public headers
├── src/
│   └── laya/                   # Implementation files
├── tests/
│   └── unit/                   # Unit tests
└── examples/                   # Example applications
```

## Build Configuration Options

### Basic Options

| Option | Default | Description |
|--------|---------|-------------|
| `LAYA_BUILD_ALL` | `${PROJECT_IS_TOP_LEVEL}` | Build tests and examples |
| `LAYA_BUILD_TESTS` | Follows `LAYA_BUILD_ALL` | Build test suite |
| `LAYA_BUILD_EXAMPLES` | Follows `LAYA_BUILD_ALL` | Build example programs |
| `LAYA_INSTALL` | `${PROJECT_IS_TOP_LEVEL}` | Generate install target |

### SDL Dependency Options

| Option | Default | Description |
|--------|---------|-------------|
| `LAYA_SDL_TARGETS_PROVIDED` | `OFF` | Skip SDL setup (parent provides SDL3) |

## Quick Start

### Building Laya Standalone

```bash
# Clone repository
git clone https://github.com/radicazz/laya.git
cd laya

# Initialize doctest submodule (for tests only)
git submodule update --init

# Configure - SDL3 will be fetched automatically
cmake -B build
cmake --build build

# Run tests
ctest --test-dir build
```

**First configure downloads SDL3 (one-time, ~30-60 seconds). Subsequent builds are fast.**

### Using Laya in Your Project

#### Option 1: FetchContent (Recommended)

```cmake
include(FetchContent)
FetchContent_Declare(
    laya
    GIT_REPOSITORY https://github.com/radicazz/laya.git
    GIT_TAG main
)
FetchContent_MakeAvailable(laya)

target_link_libraries(your_app PRIVATE laya::laya)
```

#### Option 2: Git Submodule

```bash
git submodule add https://github.com/radicazz/laya.git external/laya
git submodule update --init  # Only doctest, not SDL
```

```cmake
add_subdirectory(external/laya)
target_link_libraries(your_app PRIVATE laya::laya)
```

### Customization Examples

```cmake
# Use parent's SDL3
find_package(SDL3 REQUIRED)
set(LAYA_SDL_TARGETS_PROVIDED ON)
add_subdirectory(external/laya)

# Disable tests/examples when using as dependency
set(LAYA_BUILD_ALL OFF)
add_subdirectory(external/laya)
```

## Implementation Details

### SetupSDL3.cmake

The core SDL3 setup logic is in `cmake/SetupSDL3.cmake`:

```cmake
include(FetchContent)

# 1. Check if parent provides targets
if(LAYA_SDL_TARGETS_PROVIDED)
    # Verify required targets exist
    return()
endif()

# 2. Try system SDL3
find_package(SDL3 QUIET)

# 3. Fallback to FetchContent
if(NOT SDL3_FOUND)
    FetchContent_Declare(SDL3 ...)
    FetchContent_MakeAvailable(SDL3)
    
    # Create namespace aliases
    add_library(SDL3::SDL3 ALIAS SDL3-shared)
endif()

# Handle extensions similarly
if(LAYA_USE_SDL_IMAGE AND NOT SDL3_image_FOUND)
    FetchContent_Declare(SDL3_image ...)
    FetchContent_MakeAvailable(SDL3_image)
endif()

# Verify all targets exist
if(NOT TARGET SDL3::SDL3)
    message(FATAL_ERROR "SDL3::SDL3 target not found")
endif()
```

### Target Configuration

```cmake
# Define laya library as static
add_library(laya STATIC)
add_library(laya::laya ALIAS laya)

# Public interface
target_include_directories(laya PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link SDL3 publicly
target_link_libraries(laya PUBLIC
    SDL3::SDL3
    $<$<BOOL:${LAYA_USE_SDL_IMAGE}>:SDL3::SDL3_image>
    $<$<BOOL:${LAYA_USE_SDL_TTF}>:SDL3::SDL3_ttf>
)

# Require C++20
target_compile_features(laya PUBLIC cxx_std_20)
```

## Installation & Packaging

### LayaInstall.cmake

```cmake
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Install library and create export
install(TARGETS laya
    EXPORT LayaTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install CMake package config
install(EXPORT LayaTargets
    FILE LayaTargets.cmake
    NAMESPACE laya::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/laya
)

configure_package_config_file(...)
write_basic_package_version_file(...)
```

### Consumer Usage After Installation

```cmake
# Find installed laya
find_package(laya REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE laya::laya)
```

## Cross-Platform Considerations

### Shared Library Handling

When SDL3 is built via FetchContent as shared libraries:

**Windows:** DLLs are copied to executable directory automatically  
**Linux:** RPATH is set to `$ORIGIN` for relative library paths  
**macOS:** RPATH is set to `@loader_path` for relative library paths

This is handled by the `laya_copy_sdl_shared_libs()` function in SetupSDL3.cmake.

### Compiler Requirements

- **GCC:** 11+ (C++20 support)
- **Clang:** 15+ (C++20 support)
- **MSVC:** 2022+ (C++20 support)

## Testing

Tests use doctest (git submodule):

```bash
# Initialize doctest
git submodule update --init

# Build and run tests
cmake -B build -DLAYA_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

## Troubleshooting

### First Build is Slow

This is expected - FetchContent downloads SDL3 on first configure. Subsequent builds reuse the cached download.

### Offline Builds

Option 1: Pre-populate FetchContent cache by building once with internet  
Option 2: Install SDL3 system-wide (auto-detected via `find_package`)

### Target Not Found Errors

Ensure `LAYA_SDL_TARGETS_PROVIDED` is set correctly if your project provides SDL3 targets before adding laya.

## Success Criteria

- ✅ Automatic SDL3 dependency management via FetchContent
- ✅ Smart fallback chain (parent → system → FetchContent)
- ✅ Zero manual setup required for users
- ✅ Modern CMake best practices
- ✅ Cross-platform compatibility
- ✅ Clean target aliases (SDL3::SDL3, laya::laya)
- ✅ Both standalone and embedded usage supported
