# Building

The following document provides detailed instructions on building the Laya library, including prerequisites, build options, testing, and best practices for different use cases.

## Prerequisites

- CMake 3.21 or later
- C++20 compatible compiler (MSVC 2022, GCC 11+, Clang 15+)
- Git
- Internet connection (first build only - for downloading SDL3 via FetchContent)

## Building with Examples

```bash
# Clone the repository
git clone https://github.com/radicazz/laya.git
cd laya

# Initialize doctest submodule (only needed for tests)
git submodule update --init

# Configure and build
cmake -B build
cmake --build build --config Debug # or Release

# Run the example
build/examples/Debug/hello_laya.exe  # Windows
# or
build/examples/hello_laya            # Linux/macOS
```

**Note:** On the first configure, CMake will automatically download SDL3 from GitHub using FetchContent. This is cached, so subsequent builds will be faster.

## Build Options

```bash
# Build configuration
cmake --build build --config Debug    # Debug build
cmake --build build --config Release  # Release build

# Enable/disable tests and examples
cmake -B build -DLAYA_BUILD_TESTS=ON -DLAYA_BUILD_EXAMPLES=ON  # Default
cmake -B build -DLAYA_BUILD_TESTS=OFF  # Disable tests

# Use system-installed SDL3 instead of FetchContent
# (If SDL3 is found via find_package, it will be used automatically)
# Otherwise, FetchContent downloads SDL3
```

## Dependency Management

Laya uses a smart dependency resolution strategy:

1. **Parent-provided targets**: If your project already provides SDL3 targets, set `-DLAYA_SDL_TARGETS_PROVIDED=ON`
2. **System SDL3**: Laya automatically tries `find_package(SDL3)` first
3. **FetchContent**: If SDL3 is not found, it's automatically downloaded and built

This means:
- ✅ No manual submodule initialization for SDL
- ✅ Works out of the box with zero configuration
- ✅ Respects existing SDL3 installations
- ✅ Perfect for both standalone builds and as a dependency

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
# Build performance benchmarks (Release mode only)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target laya_benchmarks
```

---
