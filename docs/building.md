# Building

The following document provides detailed instructions on building the Laya library, including prerequisites, build options, testing, and best practices for different use cases.

## Prerequisites

- CMake 3.21 or later
- C++20 compatible compiler (MSVC 2022, GCC 11+, Clang 15+)
- Git (for submodules)

## Building with Examples

```bash
# Clone with submodules
git clone --recursive https://github.com/radicazz/laya.git
cd laya

# Configure and build
cmake -B build
cmake --build build --config Debug # or Release

# Run the example
build/examples/Debug/hello_laya.exe  # Windows
# or
build/examples/hello_laya            # Linux/macOS
```

## Build Options

```bash
# Choose SDL3 integration method
cmake -B build -DLAYA_SDL_METHOD=submodule  # Default
cmake -B build -DLAYA_SDL_METHOD=system     # System-installed SDL3

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
