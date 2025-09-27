# laya

![C++20](https://img.shields.io/badge/C%2B%2B-20-064F8C?logo=cplusplus&logoColor=white)
![SDL3](https://img.shields.io/badge/SDL-3-064F8C?logo=files&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.21-064F8C?logo=cmake&logoColor=white)
![License: MIT](https://img.shields.io/badge/License-MIT-orange?logo=open-source-initiative&logoColor=white)

<!--
[![Windows Build](https://github.com/radicazz/laya/actions/workflows/build_windows.yml/badge.svg?branch=)](https://github.com/radicazz/laya/actions/workflows/build_windows.yml)
[![Linux Build](https://github.com/radicazz/laya/actions/workflows/build_linux.yml/badge.svg?branch=)](https://github.com/radicazz/laya/actions/workflows/build_linux.yml)
-->

An intuitive wrapper that seamlessly integrates [SDL](https://github.com/libsdl-org/SDL) with modern C++.

The idea behind *laya* is to provide a clean, type-safe and idiomatic C++ interface for SDL and its extensions that feels natural while preserving the full power and flexibility of the underlying C library.

---

## Vision

By carefully wrapping SDL in a modern C++20 interface, *laya* aims to provide:

- **Performance**: Minimal overhead compared to using SDL3 directly, ensuring that applications remain fast and responsive.
- **Completeness**: Full coverage (long-term goal) of SDL3 features, ensuring that no functionality is lost in translation.
- **Ergonomics**: A clean, intuitive API that leverages C++20 features to reduce boilerplate and improve code clarity.
- **Portability**: The library should support all platforms that compile C++20.
- **Safety**: Use strong typing, RAII, and other modern C++ idioms to minimize common pitfalls like resource leaks and undefined behavior.
- **Flexibility**: Allow users to choose between linking against a system-installed SDL or using a vendored version provided by *laya*.

---

## Building

### Prerequisites

- CMake 3.21 or later
- C++20 compatible compiler (MSVC 2022, GCC 11+, Clang 14+)
- Git (for submodules)

### Quick Start

```bash
# Clone with submodules
git clone --recursive https://github.com/radicazz/laya.git
cd laya

# Configure and build
cmake -B build
cmake --build build --config Debug

# Run the example
build/examples/Debug/hello_laya.exe  # Windows
# or
build/examples/hello_laya            # Linux/macOS
```

### Build Options

```bash
# Choose SDL3 integration method
cmake -B build -DLAYA_SDL_METHOD=submodule  # Default
cmake -B build -DLAYA_SDL_METHOD=system     # System-installed SDL3
cmake -B build -DLAYA_SDL_METHOD=vcpkg      # vcpkg package manager

# Disable extensions (enabled by default)
cmake -B build -DLAYA_USE_SDL_IMAGE=OFF -DLAYA_USE_SDL_TTF=OFF

# Build configuration
cmake --build build --config Debug    # Debug build
cmake --build build --config Release  # Release build

# Enable/disable tests and examples
cmake -B build -DLAYA_BUILD_TESTS=ON -DLAYA_BUILD_EXAMPLES=ON  # Default
cmake -B build -DLAYA_BUILD_TESTS=OFF  # Disable tests
```

## Testing

laya uses [doctest](https://github.com/doctest/doctest) as its testing framework, integrated as a git submodule for easy setup.

### Running Tests

```bash
# Build with tests enabled (default)
cmake -B build -DLAYA_BUILD_TESTS=ON
cmake --build build

# Run all tests
ctest --test-dir build

# Run tests with verbose output
ctest --test-dir build --verbose

# Run specific test suites
ctest --test-dir build -R "unit"        # Unit tests only
ctest --test-dir build -R "integration" # Integration tests only

# Run tests in parallel
ctest --test-dir build --parallel 4

# Run tests directly (alternative)
build/tests/Debug/laya_tests.exe  # Windows
build/tests/laya_tests            # Linux/macOS
```

### Test Structure

- **Unit Tests**: Test individual components in isolation (`tests/unit/`)
- **Integration Tests**: Test SDL3 integration and component interactions (`tests/integration/`)
- **Performance Tests**: Benchmark performance vs raw SDL3 (`tests/performance/`)

### Test Options

```bash
# Use system-installed doctest instead of submodule
cmake -B build -DLAYA_USE_SYSTEM_DOCTEST=ON

# Build performance benchmarks (Release mode only)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target laya_benchmarks
```

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines and code style.

---

## License

This project falls under the MIT License. See [LICENSE.txt](LICENSE.txt) for details.

---
