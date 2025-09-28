# Goals

This document outlines the core goals, motivations, and vision for the laya C++20 SDL3 wrapper library.

---

## Primary Goals

### 1. **Zero-Overhead Abstraction**

Provide a modern C++ interface to SDL3 that compiles down to the same assembly as hand-written SDL3 code. The wrapper should add no runtime cost while dramatically improving compile-time safety and developer experience.

### 2. **Complete SDL3 Coverage**

Wrap 100% of SDL3's public API in a type-safe, idiomatic C++ interface. No SDL3 functionality should be inaccessible through laya, ensuring users never need to drop down to raw SDL3 calls.

### 3. **Memory Safety**

Eliminate entire classes of bugs common in SDL3 usage:

- Resource leaks (windows, textures, surfaces, etc.)
- Use-after-free errors
- Double-free errors
- Null pointer dereferences
- Buffer overruns

### 4. **Modern C++ Ergonomics**

Leverage C++20 features to create an API that feels natural to modern C++ developers:

- RAII for automatic resource management
- Strong typing to prevent logical errors
- Concepts for template constraints
- Ranges and iterators for collections
- `std::optional` and `std::expected` for error handling

### 5. **Cross-Platform Consistency**

Provide identical behavior across all platforms that support C++20 and SDL3, abstracting away platform-specific quirks while still allowing access to platform-specific features when needed.

---

## Target Audience

### Primary Users

- **Game Developers** building 2D/3D games who want modern C++ without sacrificing performance
- **Application Developers** creating multimedia applications with complex UI requirements
- **Educational Users** learning graphics programming with a safer, more approachable API

### Secondary Users

- **Library Authors** building higher-level frameworks on top of SDL3
- **Research Projects** requiring rapid prototyping with multimedia capabilities
- **Embedded Developers** working on resource-constrained systems where zero-overhead is critical

---

## Problem Statement

### What SDL3 Does Well

- Excellent cross-platform support
- High performance and low overhead
- Comprehensive feature set
- Battle-tested stability
- Active development and community

### What SDL3 Lacks

- **Type Safety**: Easy to pass wrong types to functions
- **Resource Management**: Manual memory management prone to leaks
- **Error Handling**: C-style error codes easy to ignore
- **Modern Idioms**: No RAII, ranges, or other C++ conveniences
- **Compile-Time Checks**: Many errors only caught at runtime

### How laya Solves These Problems

```cpp
// SDL3 - Error-prone, verbose
SDL_Window* window = SDL_CreateWindow("Title", 800, 600, SDL_WINDOW_RESIZABLE);
if (!window) {
    SDL_Log("Failed to create window: %s", SDL_GetError());
    return -1;
}
SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
if (!renderer) {
    SDL_DestroyWindow(window);  // Manual cleanup required
    SDL_Log("Failed to create renderer: %s", SDL_GetError());
    return -1;
}
// ... more manual cleanup needed

// laya - Safe, concise, automatic cleanup
auto window = laya::window("Title", {800, 600}, laya::window_flags::resizable);
auto renderer = laya::renderer(window);
// RAII handles all cleanup automatically
```

---

## Success Metrics

### Performance Benchmarks

- **Zero Runtime Overhead**: Assembly output identical to hand-written SDL3
- **Compile Time**: No more than 10% increase in build time vs raw SDL3
- **Binary Size**: No increase in final executable size

### API Quality Metrics

- **Type Safety**: 100% of SDL3 type errors caught at compile time
- **Resource Safety**: Zero memory leaks in comprehensive test suite
- **Ergonomics**: 50% reduction in lines of code for common tasks

### Adoption Metrics

- **Documentation**: 100% API coverage with examples
- **Platform Support**: All platforms supported by SDL3 and C++20
- **Community**: Active usage in real projects within 6 months of 1.0 release

---

## Non-Goals

### What laya Will NOT Do

#### 1. **Game Engine Features**

- Scene graphs or entity-component systems
- Physics simulation
- Asset management beyond basic loading
- Scripting language integration

#### 2. **Higher-Level Abstractions**

- UI frameworks or widget systems
- Animation systems
- Particle systems
- Networking beyond what SDL3 provides

#### 3. **Platform-Specific Extensions**

- DirectX or Metal wrappers
- Platform-specific audio APIs
- OS-specific window management beyond SDL3

#### 4. **Performance Modifications**

- Custom memory allocators
- Threading abstractions beyond SDL3
- SIMD optimizations
- GPU compute shaders

### Rationale for Non-Goals

These features belong in higher-level libraries built on top of laya. Keeping laya focused on SDL3 wrapping ensures:

- Clear project scope and maintainable codebase
- Faster development and testing cycles
- Better interoperability with existing ecosystems
- Reduced risk of feature creep and API bloat

---

## Design Philosophy

### Core Principles

#### 1. **Fail Fast, Fail Safe**

Prefer compile-time errors over runtime errors. When runtime errors are unavoidable, make them impossible to ignore.

#### 2. **Pay Only for What You Use**

Template-based design ensures unused features add zero overhead to final binaries.

#### 3. **Principle of Least Surprise**

API should behave exactly as C++ developers expect, following STL conventions and idioms.

#### 4. **Composability Over Convenience**

Provide building blocks that compose well rather than monolithic convenience functions.

#### 5. **Explicit Over Implicit**

Make resource costs and ownership clear in the API design.

---

## Long-Term Vision

### Version 1.0 (Stable Release)

- Complete SDL3 API coverage
- Comprehensive documentation and examples
- Production-ready stability
- Full platform support

### Version 2.0+ (Future Evolution)

- SDL4 support when available
- Additional optional extensions (SDL_ttf, SDL_image, SDL_mixer)
- Performance optimizations based on real-world usage
- Enhanced debugging and profiling tools

### Ecosystem Integration

- CMake find_package() support
- Package manager integration (vcpkg, Conan)
- IDE integration and IntelliSense support
- Integration with popular game development tools

---

## Conclusion

laya aims to be the definitive C++ interface to SDL3, providing safety and ergonomics without sacrificing the performance and flexibility that makes SDL3 great. By staying focused on this core mission, laya can deliver maximum value to the C++ multimedia development community.
