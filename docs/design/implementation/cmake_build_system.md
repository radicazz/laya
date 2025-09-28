# CMake Build System Implementation Plan

## Overview

This implementation plan covers the CMake infrastructure for laya v0.0.1, providing users with flexible SDL3 consumption options while maintaining modern CMake best practices.

## Goals

- Modern CMake 3.21+ with target-based design
- Support multiple SDL3 consumption methods (submodule default)
- Configurable SDL3 extension support (SDL3_image, SDL3_ttf)
- Cross-platform compatibility (Windows, Linux, macOS)
- Clean target aliases for consistent usage

## Directory Structure

```
laya/
├── CMakeLists.txt              # Root CMake configuration
├── cmake/
│   ├── LayaConfig.cmake.in     # Package config template
│   ├── FindSDL3.cmake          # SDL3 finder module
│   ├── FindSDL3_image.cmake    # SDL3_image finder
│   └── FindSDL3_ttf.cmake      # SDL3_ttf finder
├── external/
│   ├── SDL/                    # Git submodule (default)
│   ├── SDL_image/              # Git submodule
│   └── SDL_ttf/                # Git submodule
├── include/
│   └── laya/                   # Public headers
├── src/
│   └── laya/                   # Implementation files
└── tests/
    └── unit/                   # Unit tests
```

## Implementation Steps

### 1. Root CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.21)

project(laya
    VERSION 0.0.1
    DESCRIPTION "Modern C++20 wrapper for SDL3"
    LANGUAGES CXX
)

# Project options
option(LAYA_BUILD_TESTS "Build laya tests" ON)
option(LAYA_BUILD_EXAMPLES "Build laya examples" ON)
option(LAYA_INSTALL "Generate install target" ON)

# SDL3 consumption options
set(LAYA_SDL_METHOD "submodule" CACHE STRING "Method to consume SDL3")
set_property(CACHE LAYA_SDL_METHOD PROPERTY STRINGS "submodule" "system" "vcpkg")

# SDL3 extension options
option(LAYA_USE_SDL_IMAGE "Enable SDL3_image support" ON)
option(LAYA_USE_SDL_TTF "Enable SDL3_ttf support" ON)

# C++20 requirement
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add cmake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include SDL3 setup
include(cmake/SetupSDL3.cmake)

# Define laya library
add_library(laya)
add_library(laya::laya ALIAS laya)

# Configure laya target
target_include_directories(laya
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(laya
    PUBLIC
        SDL3::SDL3
        $<$<BOOL:${LAYA_USE_SDL_IMAGE}>:SDL3::SDL3_image>
        $<$<BOOL:${LAYA_USE_SDL_TTF}>:SDL3::SDL3_ttf>
)

target_compile_features(laya PUBLIC cxx_std_20)

# Add subdirectories
add_subdirectory(src)

if(LAYA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(LAYA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation
if(LAYA_INSTALL)
    include(cmake/LayaInstall.cmake)
endif()
```

### 2. cmake/SetupSDL3.cmake

```cmake
# SDL3 setup based on consumption method

if(LAYA_SDL_METHOD STREQUAL "submodule")
    # Git submodule method (default)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/CMakeLists.txt")
        message(FATAL_ERROR "SDL submodule not found. Run: git submodule update --init --recursive")
    endif()
    
    # Configure SDL3 options
    set(SDL_SHARED OFF CACHE BOOL "Build SDL3 as shared library" FORCE)
    set(SDL_STATIC ON CACHE BOOL "Build SDL3 as static library" FORCE)
    set(SDL_TEST OFF CACHE BOOL "Build SDL3 tests" FORCE)
    
    add_subdirectory(external/SDL)
    
    # Create aliases
    if(NOT TARGET SDL3::SDL3)
        add_library(SDL3::SDL3 ALIAS SDL3-static)
    endif()
    
    # Handle SDL3_image
    if(LAYA_USE_SDL_IMAGE)
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_image/CMakeLists.txt")
            message(FATAL_ERROR "SDL_image submodule not found. Run: git submodule update --init --recursive")
        endif()
        
        set(SDL3IMAGE_INSTALL OFF CACHE BOOL "" FORCE)
        add_subdirectory(external/SDL_image)
        
        if(NOT TARGET SDL3::SDL3_image)
            add_library(SDL3::SDL3_image ALIAS SDL3_image)
        endif()
    endif()
    
    # Handle SDL3_ttf
    if(LAYA_USE_SDL_TTF)
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_ttf/CMakeLists.txt")
            message(FATAL_ERROR "SDL_ttf submodule not found. Run: git submodule update --init --recursive")
        endif()
        
        set(SDL3TTF_INSTALL OFF CACHE BOOL "" FORCE)
        add_subdirectory(external/SDL_ttf)
        
        if(NOT TARGET SDL3::SDL3_ttf)
            add_library(SDL3::SDL3_ttf ALIAS SDL3_ttf)
        endif()
    endif()

elseif(LAYA_SDL_METHOD STREQUAL "system")
    # System-installed SDL3
    find_package(SDL3 REQUIRED)
    
    if(LAYA_USE_SDL_IMAGE)
        find_package(SDL3_image REQUIRED)
    endif()
    
    if(LAYA_USE_SDL_TTF)
        find_package(SDL3_ttf REQUIRED)
    endif()

elseif(LAYA_SDL_METHOD STREQUAL "vcpkg")
    # vcpkg package manager
    find_package(SDL3 CONFIG REQUIRED)
    
    if(LAYA_USE_SDL_IMAGE)
        find_package(SDL3-image CONFIG REQUIRED)
        if(NOT TARGET SDL3::SDL3_image)
            add_library(SDL3::SDL3_image ALIAS SDL3_image::SDL3_image)
        endif()
    endif()
    
    if(LAYA_USE_SDL_TTF)
        find_package(SDL3-ttf CONFIG REQUIRED)
        if(NOT TARGET SDL3::SDL3_ttf)
            add_library(SDL3::SDL3_ttf ALIAS SDL3_ttf::SDL3_ttf)
        endif()
    endif()

else()
    message(FATAL_ERROR "Unknown SDL method: ${LAYA_SDL_METHOD}")
endif()

# Verify required targets exist
if(NOT TARGET SDL3::SDL3)
    message(FATAL_ERROR "SDL3::SDL3 target not found")
endif()

if(LAYA_USE_SDL_IMAGE AND NOT TARGET SDL3::SDL3_image)
    message(FATAL_ERROR "SDL3::SDL3_image target not found")
endif()

if(LAYA_USE_SDL_TTF AND NOT TARGET SDL3::SDL3_ttf)
    message(FATAL_ERROR "SDL3::SDL3_ttf target not found")
endif()
```

### 3. cmake/LayaInstall.cmake

```cmake
# Installation configuration

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Install library
install(TARGETS laya
    EXPORT LayaTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install export targets
install(EXPORT LayaTargets
    FILE LayaTargets.cmake
    NAMESPACE laya::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/laya
)

# Generate config file
configure_package_config_file(
    cmake/LayaConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/LayaConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/laya
)

# Generate version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/LayaConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install config files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/LayaConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/LayaConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/laya
)
```

### 4. cmake/LayaConfig.cmake.in

```cmake
@PACKAGE_INIT@

include(CMakeFindDependencyMacro)

# Find SDL3 dependencies
find_dependency(SDL3 REQUIRED)

if(@LAYA_USE_SDL_IMAGE@)
    find_dependency(SDL3_image REQUIRED)
endif()

if(@LAYA_USE_SDL_TTF@)
    find_dependency(SDL3_ttf REQUIRED)
endif()

# Include targets
include("${CMAKE_CURRENT_LIST_DIR}/LayaTargets.cmake")

check_required_components(laya)
```

## Quick Start Guides

### Method 1: Git Submodules (Default)

```bash
# Clone with submodules
git clone --recursive https://github.com/user/laya.git
cd laya

# Or initialize submodules after cloning
git submodule update --init --recursive

# Configure and build
cmake -B build -DLAYA_SDL_METHOD=submodule
cmake --build build

# Optional: disable extensions
cmake -B build -DLAYA_USE_SDL_IMAGE=OFF -DLAYA_USE_SDL_TTF=OFF
```

### Method 2: System Installation

```bash
# Install SDL3 system-wide first
# Ubuntu/Debian: sudo apt install libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev
# macOS: brew install sdl3 sdl3_image sdl3_ttf
# Windows: vcpkg install sdl3 sdl3-image sdl3-ttf

# Configure and build
cmake -B build -DLAYA_SDL_METHOD=system
cmake --build build
```

### Method 3: vcpkg Package Manager

```bash
# Install dependencies via vcpkg
vcpkg install sdl3 sdl3-image sdl3-ttf

# Configure with vcpkg toolchain
cmake -B build -DLAYA_SDL_METHOD=vcpkg -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg.cmake
cmake --build build
```

## Consumer Usage

```cmake
# In consumer CMakeLists.txt
find_package(laya REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE laya::laya)
```

## Testing Strategy

### Unit Tests Structure

```
tests/
├── CMakeLists.txt
├── unit/
│   ├── test_init.cpp
│   ├── test_window.cpp
│   └── test_renderer.cpp
└── integration/
    └── test_basic_app.cpp
```

### Test CMakeLists.txt

```cmake
find_package(doctest REQUIRED)

add_executable(laya_tests
    unit/test_init.cpp
    unit/test_window.cpp
    unit/test_renderer.cpp
)

target_link_libraries(laya_tests
    PRIVATE
        laya::laya
        doctest::doctest
)

add_test(NAME laya_unit_tests COMMAND laya_tests)
```

## Implementation Timeline

### Week 1: Core CMake Infrastructure

- [ ] Create root CMakeLists.txt with basic structure
- [ ] Implement SetupSDL3.cmake for submodule method
- [ ] Add basic library target with aliases
- [ ] Test submodule integration

### Week 2: Multi-Method Support

- [ ] Add system installation method support
- [ ] Add vcpkg method support
- [ ] Create finder modules for system method
- [ ] Test all three consumption methods

### Week 3: Extension Support

- [ ] Implement SDL3_image integration
- [ ] Implement SDL3_ttf integration
- [ ] Add configuration options for extensions
- [ ] Test extension combinations

### Week 4: Installation & Packaging

- [ ] Create LayaInstall.cmake
- [ ] Implement package config generation
- [ ] Test find_package() functionality
- [ ] Create installation documentation

## Success Criteria

- [ ] All three SDL consumption methods work correctly
- [ ] Extensions can be enabled/disabled independently
- [ ] Modern CMake best practices followed
- [ ] Cross-platform compatibility verified
- [ ] Clean target aliases provided
- [ ] Installation and packaging functional
- [ ] Consumer projects can easily integrate laya

## Risk Mitigation

### SDL3 API Instability

- Pin specific SDL3 commit in submodules
- Version detection in CMake scripts
- Compatibility layer for API changes

### Platform-Specific Issues

- Test on Windows, Linux, macOS
- Handle different compiler requirements
- Address linking differences per platform

### Dependency Management

- Clear error messages for missing dependencies
- Fallback options for optional components
- Version compatibility checks
