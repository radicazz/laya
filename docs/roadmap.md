# Roadmap

> [!IMPORTANT]
> This document is a work in progress and subject to change.

Here is a comprehensive roadmap for the development of the project, a modern C++20 wrapper for SDL3. It outlines the  evolution from initial setup through stable release, with detailed milestones and feature implementations.

---

## Project Vision

laya aims to be a definitive C++20 interface to SDL3, providing:

- **Lowest-cost abstractions** that add minimal overhead
- **Modern C++ safety** with RAII, strong typing, and exception handling
- **Complete SDL3 coverage** ensuring no functionality is lost (long-term goal)
- **Intuitive APIs** that feel natural to C++ developers

---

## 🚀 Phase 1: Foundation (v0.0.x)

**Goal**: Establish robust development infrastructure

### 1. Project Creation ✅

- [x] Initialize repository with `LICENSE`, `README`, `CONTRIBUTING`, style guide
- [x] Add `.gitignore`, `.editorconfig`, `clang-format`, and other config files
- [x] Create comprehensive `docs/` structure with planning documents
- [x] Establish project philosophy and design principles

### 2. Build System ✅

- [x] Create root `CMakeLists.txt` with C++20 support
- [x] Implement SDL3 integration options (submodule, system, vcpkg)
- [x] Configure modern CMake 3.21+ with target-based design
- [x] Set up cross-platform build support (Windows tested, Linux/macOS ready)
- [x] Create minimal "Hello laya" example for build validation
- [x] Add SDL3_image and SDL3_ttf submodule support (core ready, extensions for Week 3)

### 3. Testing Framework ✅

- [x] Integrate doctest testing framework
- [x] Create comprehensive test structure (unit, integration, performance)
- [x] Set up CTest integration
- [x] Write initial framework validation tests

**Test Structure**:

```bash
tests/
├── unit/           # Component-specific tests
├── integration/    # Cross-component tests  
├── performance/    # Benchmarks vs raw SDL3
└── utils/          # Utility functions and helpers
```

### 4. Continuous Integration ✅

- [x] GitHub Actions workflows for all platforms
- [x] Multi-compiler matrix (GCC, Clang, MSVC)
- [x] Dependency caching for faster builds
- [x] Coverage reporting integration
- [x] Automated artifact generation

**CI Matrix**:

Status | Platform | Compilers | Build Types |
----------|-----------|-------------|-------------|
✅ | Ubuntu | GCC 11+, Clang 15+ | Debug, Release |
✅ | Windows | MSVC 2022, Clang | Debug, Release |
❌️ | macOS | Clang 15+ | Debug, Release |

> [!NOTE]
> Current CI supports MSVC, GCC, and Clang - all the major compilers. macOS is theoretically supported, but not yet tested.

### 5. Planning & Design ✅

- [x] Complete feature mapping and prioritization
- [x] Document C++20 wrapping techniques
- [x] Create detailed implementation plans
- [x] Establish API design mockups
- [x] Define success metrics and benchmarks

---

## 🛠️ Phase 2: Core Systems (v0.1.x - v0.4.x)

**Goal**: Implement fundamental SDL3 wrappers

### v0.1.0: Initialization & Error Handling 🔴

**Priority**: Critical  

**Core Features**:

- [ ] `laya::context` RAII class for SDL initialization
- [ ] `laya::subsystem` type-safe enum with bitwise operations
- [ ] `laya::error` exception class with SDL integration
- [ ] Thread-safe initialization/shutdown
- [ ] Version information queries

**API Preview**:

```cpp
laya::context ctx{laya::subsystem::video, laya::subsystem::audio};
auto version = laya::context::version();
```

**Success Criteria**:

- Zero memory leaks in initialization/shutdown
- Thread-safe operation verified
- Exception safety guaranteed
- Performance matches raw SDL3

### v0.2.0: Window Management 🔴

**Priority**: Critical  

**Core Features**:

- [ ] `laya::window` RAII class with move semantics
- [ ] `laya::window_flags` type-safe enum
- [ ] Strong types for `size`, `position`
- [ ] Window property management (title, size, position, state)
- [ ] Multi-monitor support

**API Preview**:

```cpp
laya::window win("Title", {800, 600}, 
    laya::window_flags::resizable | laya::window_flags::borderless);
win.set_position({100, 100});
auto current_size = win.size();
```

**Success Criteria**:

- All window operations work correctly
- Move semantics prevent resource leaks
- Multi-monitor functionality verified

### v0.3.0: 2D Rendering 🔴

**Priority**: Critical  

**Core Features**:

- [ ] `laya::renderer` RAII class
- [ ] `laya::color` with compile-time utilities
- [ ] Geometric primitives (`point`, `rect`, `line`)
- [ ] Template-based drawing functions
- [ ] Batch rendering support

**API Preview**:

```cpp
laya::renderer ren(window);
ren.clear(laya::color::from_hex(0x2E3440));
ren.set_draw_color(laya::color::red());
ren.draw_points(point_container);
ren.present();
```

**Success Criteria**:

- Zero rendering overhead vs raw SDL3
- Template functions compile correctly
- Color system is intuitive and complete

### v0.4.0: Event Handling 🔴

**Priority**: Critical  

**Core Features**:

- [ ] Type-safe event system with `std::variant`
- [ ] Range-based event polling
- [ ] Event filtering utilities
- [ ] Input state queries (keyboard, mouse)

**API Preview**:

```cpp
for (const auto& event : laya::poll_events()) {
    std::visit([&](const auto& e) {
        using T = std::decay_t<decltype(e)>;
        if constexpr (std::is_same_v<T, laya::quit_event>) {
            running = false;
        }
    }, event);
}
```

**Success Criteria**:

- All SDL events properly converted
- Range interface is intuitive
- Event filtering is efficient

---

## 🎨 Phase 2: Graphics & Media (v0.5.x - v0.6.x)

**Goal**: Complete multimedia capabilities

### v0.5.0: Textures & Surfaces 🟡

**Priority**: Important  

**Core Features**:

- [ ] `laya::texture` and `laya::surface` RAII wrappers
- [ ] Image loading integration (BMP built-in, optional SDL_image)
- [ ] Texture rendering with transforms
- [ ] Pixel format handling

**Success Criteria**:

- Safe texture/surface management
- Image loading works correctly
- Performance matches raw SDL3

### v0.6.0: Audio System 🟡

**Priority**: Important  

**Core Features**:

- [ ] `laya::audio_device` RAII wrapper
- [ ] Audio format specification
- [ ] Basic playback and streaming
- [ ] Volume and pan control

**Success Criteria**:

- Audio devices managed safely
- Basic playback functional
- Streaming works correctly

---

## 🎮 Phase 3: Input Systems (v0.7.x)

**Goal**: Complete input handling

### v0.7.0: Advanced Input 🟡

**Priority**: Important  

**Core Features**:

- [ ] `laya::gamepad` support with haptic feedback
- [ ] Advanced keyboard/mouse features
- [ ] Text input handling
- [ ] Clipboard integration

**Success Criteria**:

- Gamepad input works correctly
- Text input properly handled
- Clipboard operations functional

---

## 🚀 Phase 4: Advanced Features (v0.8.x - v0.9.x)

**Goal**: Modern graphics and system integration

### v0.8.0: GPU Rendering 🟢

**Priority**: Nice-to-Have  

**Core Features**:

- [ ] SDL3 GPU API wrapper
- [ ] Modern graphics pipeline support
- [ ] Shader management
- [ ] GPU resource management

**Success Criteria**:

- GPU pipeline functional
- Shader system works correctly
- Performance is competitive

### v0.9.0: Threading & File I/O 🟢

**Priority**: Nice-to-Have  

**Core Features**:

- [ ] Thread-safe wrappers
- [ ] `std::thread` integration
- [ ] File I/O streams
- [ ] Path utilities

**Success Criteria**:

- Threading utilities are safe
- File operations work correctly

---

## 🎯 Phase 5: Polish & Release (v1.0.0)

**Goal**: Production-ready stable release

### v1.0.0: Stable Release 🟢

**Priority**: Critical for Release

**Documentation & Examples**:

- [ ] Complete API documentation with Doxygen
- [ ] Tutorial examples and guides
- [ ] Performance benchmarking results
- [ ] Migration guides from raw SDL3

**Packaging & Distribution**:

- [ ] CMake package configuration
- [ ] vcpkg and Conan integration
- [ ] GitHub Releases automation
- [ ] Binary artifact generation

**Quality Assurance**:

- [ ] Comprehensive test coverage (>90%)
- [ ] Memory leak validation
- [ ] Performance verification (<5% overhead)
- [ ] Cross-platform testing

---

## 🔮 Future Roadmap (v1.1.0+)

### Extension Libraries 🔵

**v1.1.0 - SDL_image Support**:

- PNG, JPEG, WEBP loading
- Type-safe format detection
- Integration with laya texture system

**v1.2.0 - SDL_ttf Support**:

- Font loading and management
- Text rendering to surfaces/textures
- Unicode support

**v1.3.0 - SDL_mixer Support**:

- Audio mixing and effects
- Music and sound effect playback
- 3D audio positioning

### Advanced Features 🔵

**v1.4.0 - Performance Optimizations**:

- Custom allocators
- SIMD optimizations where beneficial
- Profile-guided optimization

**v1.5.0 - Developer Tools**:

- Debug visualization tools
- Performance profiling integration
- Memory usage analysis

---

## 🎯 Dependency Graph

```bash
Foundation Phase
    ↓
Initialization (v0.1.0)
    ↓
Window Management (v0.2.0)
    ↓
┌─ 2D Rendering (v0.3.0) ─→ Textures (v0.5.0) ─→ GPU Rendering (v0.8.0)
│                    ↓
└─ Event Handling (v0.4.0) ─→ Advanced Input (v0.7.0)
                    ↓
                Audio (v0.6.0)
                    ↓
                Threading & I/O (v0.9.0)
                    ↓
                Stable Release (v1.0.0)
```

---

## 🚨 Risk Mitigation

### High-Risk Items

1. **SDL3 API Stability**: Track development closely, maintain compatibility layer
2. **C++20 Compiler Support**: Test multiple compiler versions, document requirements
3. **Performance Overhead**: Continuous benchmarking, zero-overhead design principles

### Contingency Plans

- **SDL3 Changes**: Version detection and compatibility shims
- **Compiler Issues**: Fallback implementations for missing features
- **Performance Problems**: Profile-guided optimization and template specialization

---

## ✅ Progress Tracking

This roadmap serves as a living document that will be updated as development progresses. Each milestone includes specific success criteria and will be marked complete only when all requirements are met.

**Current Status**: Foundation Phase Complete ✅

**Next Milestone**: Testing Framework Setup (Week 2-3)

For detailed implementation plans, see the `design/implementation_plans/` directory.
For API usage examples, see the `design/mockups/` directory.
For technical details, see `design/wrapping_techniques.md`.
