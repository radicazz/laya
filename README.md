# 🧱 laya

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
```

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines and code style.

---

## License

This project falls under the MIT License. See [LICENSE.txt](LICENSE.txt) for details.

---
