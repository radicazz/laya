# Roadmap

> [!IMPORTANT]
> This document is a work in progress and subject to change.

This document serves as a comprehensive roadmap for the development of laya, a modern C++20 wrapper for SDL3. It outlines the project's evolution from initial setup through stable release, with detailed milestones and feature implementations.

---

## Project Vision

laya aims to be the definitive C++20 interface to SDL3, providing:

- **Zero-overhead abstractions** that compile to identical assembly as raw SDL3
- **Modern C++ safety** with RAII, strong typing, and exception handling
- **Complete SDL3 coverage** ensuring no functionality is lost
- **Intuitive APIs** that feel natural to C++ developers

---

## 🚀 Phase 0: Foundation (v0.0.x)

**Timeline**: Weeks 1-3  
**Goal**: Establish robust development infrastructure

### 1. Project Creation ✅

- [x] Initialize repository with `LICENSE`, `README`, `CONTRIBUTING`, style guide
- [x] Add `.gitignore`, `.editorconfig`, `clang-format`, and other config files
- [x] Create comprehensive `docs/` structure with planning documents
- [x] Establish project philosophy and design principles

### 2. Build System ✅

**Target**: Week 2  
**Priority**: Critical

- [x] Create root `CMakeLists.txt` with C++20 support
- [x] Implement SDL3 integration options (submodule, system, vcpkg)
- [x] Configure modern CMake 3.21+ with target-based design
- [x] Set up cross-platform build support (Windows tested, Linux/macOS ready)
- [x] Create minimal "Hello laya" example for build validation
- [x] Add SDL3_image and SDL3_ttf submodule support (core ready, extensions for Week 3)

**Success Criteria**: ✅ COMPLETED

- ✅ Builds successfully on Windows (other platforms ready)
- ✅ Can build SDL3 via submodules (default method)
- ✅ Modern CMake best practices implemented
- ✅ Working example application validates integration

### 3. Testing Framework 🟡

**Target**: Week 2-3  
**Priority**: Critical

- [ ] Integrate doctest testing framework
- [ ] Create comprehensive test structure (unit, integration, performance)
- [ ] Set up CTest integration
- [ ] Write initial framework validation tests

**Test Structure**:

```bash
tests/
├── unit/           # Component-specific tests
├── integration/    # Cross-component tests  
├── performance/    # Benchmarks vs raw SDL3
└── examples/       # Example code validation
```

### 4. Continuous Integration 🟡

**Target**: Week 3  
**Priority**: High

- [ ] GitHub Actions workflows for all platforms
- [ ] Multi-compiler matrix (GCC, Clang, MSVC)
- [ ] Dependency caching for faster builds
- [ ] Coverage reporting integration
- [ ] Automated artifact generation

**CI Matrix**:

| Platform | Compilers | Build Types |
|----------|-----------|-------------|
| Ubuntu | GCC 11+, Clang 14+ | Debug, Release |
| Windows | MSVC 2022, Clang | Debug, Release |
| macOS | Clang 14+ | Debug, Release |

### 5. Planning & Design ✅

- [x] Complete feature mapping and prioritization
- [x] Document C++20 wrapping techniques
- [x] Create detailed implementation plans
- [x] Establish API design mockups
- [x] Define success metrics and benchmarks

---

## 🛠️ Phase 1: Core Systems (v0.1.x - v0.4.x)

**Timeline**: Weeks 4-12  
**Goal**: Implement fundamental SDL3 wrappers

### v0.1.0: Initialization & Error Handling 🔴

**Target**: Week 4  
**Priority**: Critical  
**Effort**: 5-7 days

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

**Target**: Week 6  
**Priority**: Critical  
**Effort**: 7-10 days

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

**Target**: Week 9  
**Priority**: Critical  
**Effort**: 10-14 days

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

**Target**: Week 12  
**Priority**: Critical  
**Effort**: 12-16 days

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

**Timeline**: Weeks 13-17  
**Goal**: Complete multimedia capabilities

### v0.5.0: Textures & Surfaces 🟡

**Target**: Week 15  
**Priority**: Important  
**Effort**: 8-12 days

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

**Target**: Week 17  
**Priority**: Important  
**Effort**: 10-14 days

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

**Timeline**: Weeks 18-19  
**Goal**: Complete input handling

### v0.7.0: Advanced Input 🟡

**Target**: Week 19  
**Priority**: Important  
**Effort**: 8-10 days

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

**Timeline**: Weeks 20-24  
**Goal**: Modern graphics and system integration

### v0.8.0: GPU Rendering 🟢

**Target**: Week 22  
**Priority**: Nice-to-Have  
**Effort**: 15-20 days

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

**Target**: Week 24  
**Priority**: Nice-to-Have  
**Effort**: 6-8 days

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

**Timeline**: Weeks 25-27  
**Goal**: Production-ready stable release

### v1.0.0: Stable Release 🟢

**Target**: Week 27  
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

## 📊 Success Metrics

### Development Metrics

- **Code Coverage**: >90% for core functionality
- **Build Time**: <5 minutes for full build
- **Test Execution**: <30 seconds for full test suite

### Quality Metrics

- **Memory Leaks**: Zero in comprehensive test suite
- **Performance Overhead**: <5% compared to raw SDL3
- **API Completeness**: >95% SDL3 coverage by v1.0

### Community Metrics

- **Documentation Coverage**: 100% public API
- **Example Coverage**: All major use cases
- **Issue Response Time**: <48 hours

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

**Current Status**: Foundation Phase - Build System Complete ✅

**Next Milestone**: Testing Framework Setup (Week 2-3)

For detailed implementation plans, see the `docs/implementation_plans/` directory.
For API usage examples, see the `docs/mockups/` directory.
For technical details, see `docs/wrapping_techniques.md`.
