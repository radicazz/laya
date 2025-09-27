# Project Plan

This document provides a detailed, step-by-step breakdown of tasks required to implement the laya C++20 SDL3 wrapper library from initial setup through stable release.

---

## Phase 0: Foundation (v0.0.x)

### 0.1 Repository Infrastructure ✅

**Status**: Complete  
**Duration**: 1-2 days

- [x] Initialize Git repository with proper structure
- [x] Create LICENSE.txt (MIT)
- [x] Create README.md with project vision
- [x] Create CONTRIBUTING.md with style guidelines
- [x] Add .gitignore for C++/CMake projects
- [x] Add .editorconfig for consistent formatting
- [x] Add .clang-format for code style enforcement

### 0.2 Build System Setup ✅

**Status**: Complete  
**Duration**: 3-5 days  
**Dependencies**: Repository Infrastructure

#### Tasks

- [x] **Create root CMakeLists.txt**
  - CMake version 3.21+ (for modern C++20 support)
  - Project declaration with version 0.0.1
  - C++20 standard requirement with feature detection
  - Cross-platform support (Windows tested, Linux/macOS ready)

- [x] **SDL3 Integration Options**
  - ✅ Git submodule integration (default method)
  - ✅ System-installed SDL3 via find_package()
  - ✅ vcpkg package manager support
  - ✅ CMake option to choose integration method (LAYA_SDL_METHOD)

- [x] **Directory Structure**

  ```
  laya/
  ├── CMakeLists.txt            # Root configuration ✅
  ├── cmake/
  │   ├── SetupSDL3.cmake       # SDL3 integration ✅
  │   ├── LayaInstall.cmake     # Installation config ✅
  │   └── LayaConfig.cmake.in   # Package config ✅
  ├── include/
  │   └── laya/
  │       └── init.hpp          # Basic init API ✅
  ├── src/
  │   ├── CMakeLists.txt        # Source config ✅
  │   └── laya/
  │       └── init.cpp          # Implementation ✅
  ├── tests/
  │   └── CMakeLists.txt        # Test placeholder ✅
  ├── examples/
  │   ├── CMakeLists.txt        # Examples config ✅
  │   └── hello_laya.cpp        # Working example ✅
  ├── external/                 # Git submodules ✅
  │   ├── SDL/                  # SDL3 submodule ✅
  │   ├── SDL_image/            # SDL3_image submodule ✅
  │   └── SDL_ttf/              # SDL3_ttf submodule ✅
  └── docs/                     # Documentation ✅
  ```

- [x] **Compiler Configuration**
  - ✅ C++20 features enabled with target_compile_features()
  - ✅ Modern CMake target-based design
  - ✅ Static library configuration
  - ✅ Clean target aliases (laya::laya, SDL3::SDL3)

#### Success Criteria ✅

- ✅ CMake configures successfully on Windows (other platforms ready)
- ✅ Can build SDL3 via submodules (default method)
- ✅ Builds working "hello laya" example
- ✅ Modern CMake best practices implemented
- ✅ Extension support ready (SDL3_image, SDL3_ttf for Week 3)

### 0.3 Testing Framework

**Status**: Not Started  
**Duration**: 2-3 days  
**Dependencies**: Build System Setup

#### Tasks

- [ ] **Choose Testing Framework**
  - Evaluate: doctest, Catch2, GoogleTest
  - Recommendation: doctest (header-only, fast compilation)

- [ ] **Integrate Testing Framework**
  - Add testing framework via CMake
  - Create tests/CMakeLists.txt
  - Set up test discovery and execution
  - Configure CTest integration

- [ ] **Create Test Structure**

  ```
  tests/
  ├── CMakeLists.txt
  ├── unit/                    # Unit tests for individual components
  │   ├── core/
  │   ├── graphics/
  │   └── input/
  ├── integration/             # Integration tests
  └── performance/             # Performance benchmarks
  ```

- [ ] **Write Initial Tests**
  - Test framework integration test
  - SDL3 linking verification test
  - Basic compilation tests for headers

#### Success Criteria

- Tests compile and run successfully
- Test results integrate with CMake/CTest
- CI can discover and execute tests

### 0.4 Continuous Integration

**Status**: Not Started  
**Duration**: 3-4 days  
**Dependencies**: Testing Framework

#### Tasks

- [ ] **GitHub Actions Workflows**
  - `.github/workflows/ci.yml` for main CI pipeline
  - `.github/workflows/release.yml` for release automation

- [ ] **Multi-Platform CI Matrix**

  ```yaml
  strategy:
    matrix:
      os: [ubuntu-latest, windows-latest, macos-latest]
      compiler: [gcc, clang, msvc]
      build_type: [Debug, Release]
      exclude:
        - os: windows-latest
          compiler: gcc
        - os: macos-latest
          compiler: msvc
  ```

- [ ] **CI Pipeline Steps**
  1. Checkout code
  2. Install dependencies (SDL3, CMake, compiler)
  3. Configure CMake
  4. Build project
  5. Run tests
  6. Upload artifacts
  7. Generate coverage reports (for Debug builds)

- [ ] **Dependency Caching**
  - Cache SDL3 builds
  - Cache CMake configuration
  - Cache package manager downloads

#### Success Criteria

- All target platforms build successfully
- Tests pass on all platforms
- Build artifacts are generated
- Coverage reports are available

---

## Phase 1: Core Systems (v0.1.x - v0.4.x)

### 1.1 Initialization & Error Handling (v0.1.0)

**Status**: Not Started  
**Duration**: 5-7 days  
**Dependencies**: Foundation Phase

#### Tasks

- [ ] **Core Headers Structure**

  ```cpp
  // include/laya/core/init.hpp
  // include/laya/core/error.hpp
  // include/laya/core/types.hpp
  ```

- [ ] **Error Handling System**
  - Define `laya::error` exception class
  - Implement `from_sdl()` static method
  - Create error handling macros/utilities
  - Thread-local error state management

- [ ] **Initialization System**
  - `laya::context` RAII class for SDL initialization
  - `laya::subsystem` enum class
  - Convenience `init()` and `quit()` functions
  - Subsystem dependency tracking

- [ ] **Type Definitions**
  - Basic geometric types (point, size, rect)
  - Color type with compile-time utilities
  - Strong typing for SDL constants

#### Implementation Details

```cpp
// Core error handling
namespace laya {
    class error : public std::runtime_error {
    public:
        explicit error(const std::string& msg);
        static error from_sdl();
    };
    
    enum class subsystem {
        timer, audio, video, joystick, haptic, 
        gamepad, events, sensor, camera
    };
    
    class context {
    public:
        explicit context(std::initializer_list<subsystem> systems = {subsystem::video});
        ~context();
        // Non-copyable, movable
    };
}
```

#### Tests

- [ ] Error handling tests
- [ ] Initialization/shutdown tests
- [ ] RAII context tests
- [ ] Subsystem dependency tests

#### Success Criteria

- All tests pass
- No memory leaks in initialization/shutdown
- Exception safety guaranteed
- Documentation complete

### 1.2 Window Management (v0.2.0)

**Status**: Not Started  
**Duration**: 7-10 days  
**Dependencies**: Initialization & Error Handling

#### Tasks

- [ ] **Window Class Design**

  ```cpp
  // include/laya/graphics/window.hpp
  namespace laya {
      enum class window_flags : uint32_t { /* ... */ };
      
      class window {
      public:
          window(std::string_view title, size sz, window_flags flags = window_flags::none);
          ~window();
          
          // Non-copyable, movable
          window(const window&) = delete;
          window& operator=(const window&) = delete;
          window(window&&) noexcept;
          window& operator=(window&&) noexcept;
          
          // Property accessors
          void set_title(std::string_view title);
          std::string_view title() const;
          void set_size(size sz);
          size size() const;
          void set_position(position pos);
          position position() const;
          
          // State management
          void show();
          void hide();
          void maximize();
          void minimize();
          void restore();
          
          // Interop
          SDL_Window* native_handle() const;
      };
  }
  ```

- [ ] **Window Flags System**
  - Type-safe enum class for window flags
  - Bitwise operators for flag combinations
  - Validation of flag combinations

- [ ] **Multi-Monitor Support**
  - Display enumeration utilities
  - Window positioning relative to displays
  - DPI awareness handling

#### Tests

- [ ] Window creation/destruction tests
- [ ] Window property modification tests
- [ ] Window state management tests
- [ ] Multi-monitor tests
- [ ] RAII and move semantics tests

#### Success Criteria

- Windows create and destroy properly
- All window properties work correctly
- Move semantics work without resource leaks
- Multi-monitor support functional

### 1.3 2D Rendering (v0.3.0)

**Status**: Not Started  
**Duration**: 10-14 days  
**Dependencies**: Window Management

#### Tasks

- [ ] **Renderer Class Design**

  ```cpp
  // include/laya/graphics/renderer.hpp
  namespace laya {
      class renderer {
      public:
          explicit renderer(const window& win);
          ~renderer();
          
          // Non-copyable, movable
          renderer(const renderer&) = delete;
          renderer& operator=(const renderer&) = delete;
          renderer(renderer&&) noexcept;
          renderer& operator=(renderer&&) noexcept;
          
          // Basic rendering
          void clear(color c = color::black());
          void present();
          
          // Drawing state
          void set_draw_color(color c);
          color draw_color() const;
          
          // Primitive drawing
          void draw_point(point p);
          void draw_line(point start, point end);
          void draw_rect(rect r);
          void fill_rect(rect r);
          
          // Advanced drawing (template overloads)
          template<typename Container>
          void draw_points(const Container& points);
          
          template<typename Container>
          void draw_lines(const Container& lines);
          
          SDL_Renderer* native_handle() const;
      };
  }
  ```

- [ ] **Color System**
  - RGBA color struct with compile-time utilities
  - Common color constants
  - Color space conversion utilities
  - HSV/HSL support

- [ ] **Geometric Primitives**
  - Strong types for point, rect, line
  - Geometric utility functions
  - Template support for user-defined types

#### Tests

- [ ] Renderer creation/destruction tests
- [ ] Basic drawing operation tests
- [ ] Color system tests
- [ ] Template overload tests
- [ ] Performance benchmarks vs raw SDL

#### Success Criteria

- All basic drawing operations work
- Performance matches raw SDL
- Template overloads compile and work correctly
- Color system is intuitive and complete

### 1.4 Event Handling (v0.4.0)

**Status**: Not Started  
**Duration**: 12-16 days  
**Dependencies**: Window Management

#### Tasks

- [ ] **Event System Design**

  ```cpp
  // include/laya/input/events.hpp
  namespace laya {
      // Individual event types
      struct quit_event { uint32_t timestamp; };
      
      struct key_event {
          enum class state { pressed, released };
          uint32_t timestamp;
          state key_state;
          // ... key details
      };
      
      struct mouse_button_event {
          enum class state { pressed, released };
          enum class button { left, right, middle };
          uint32_t timestamp;
          state button_state;
          button mouse_button;
          point position;
          uint8_t clicks;
      };
      
      // Event variant
      using event = std::variant<
          quit_event,
          key_event,
          mouse_button_event
          // ... other event types
      >;
      
      // Event polling
      class event_range {
          // Iterator implementation
      };
      
      event_range poll_events();
      std::optional<event> wait_event();
      std::optional<event> wait_event_timeout(std::chrono::milliseconds timeout);
  }
  ```

- [ ] **Event Conversion System**
  - Convert SDL_Event to laya::event
  - Type-safe event variant handling
  - Comprehensive event type coverage

- [ ] **Range-Based Event Interface**
  - Iterator-based event polling
  - Range-for loop support
  - Event filtering utilities

- [ ] **Input State Queries**
  - Keyboard state queries
  - Mouse state queries
  - Modifier key handling

#### Tests

- [ ] Event conversion tests
- [ ] Event polling tests
- [ ] Range interface tests
- [ ] Input state query tests
- [ ] Event filtering tests

#### Success Criteria

- All SDL events properly converted
- Range-based interface works intuitively
- Event filtering is efficient
- Input state queries are accurate

---

## Phase 2: Graphics & Media (v0.5.x - v0.6.x)

### 2.1 Textures & Surfaces (v0.5.0)

**Status**: Not Started  
**Duration**: 8-12 days  
**Dependencies**: 2D Rendering

#### Tasks

- [ ] **Texture System**
  - RAII texture wrapper
  - Texture creation from surfaces
  - Texture rendering with transforms
  - Texture property queries

- [ ] **Surface System**
  - RAII surface wrapper
  - Surface creation and manipulation
  - Pixel format handling
  - Surface-to-texture conversion

- [ ] **Image Loading Integration**
  - Basic BMP support (SDL built-in)
  - Optional SDL_image integration
  - Type-safe format detection

#### Success Criteria

- Textures and surfaces managed safely
- Image loading works correctly
- Performance matches raw SDL

### 2.2 Audio System (v0.6.0)

**Status**: Not Started  
**Duration**: 10-14 days  
**Dependencies**: Core Systems

#### Tasks

- [ ] **Audio Device Management**
  - RAII audio device wrapper
  - Device enumeration
  - Audio format specification

- [ ] **Audio Playback**
  - Basic audio playback
  - Audio streaming
  - Volume and pan control

#### Success Criteria

- Audio devices managed safely
- Basic audio playback functional
- Audio streaming works correctly

---

## Phase 3: Input Systems (v0.7.x)

### 3.1 Advanced Input (v0.7.0)

**Status**: Not Started  
**Duration**: 8-10 days  
**Dependencies**: Event Handling

#### Tasks

- [ ] **Gamepad Support**
  - Gamepad enumeration and management
  - Button and axis input
  - Haptic feedback

- [ ] **Advanced Keyboard/Mouse**
  - Text input handling
  - Cursor management
  - Clipboard integration

#### Success Criteria

- Gamepad input works correctly
- Text input is properly handled
- Cursor and clipboard operations work

---

## Phase 4: Advanced Features (v0.8.x - v0.9.x)

### 4.1 GPU Rendering (v0.8.0)

**Status**: Not Started  
**Duration**: 15-20 days  
**Dependencies**: 2D Rendering

#### Tasks

- [ ] **SDL3 GPU API Wrapper**
  - Modern graphics pipeline wrapper
  - Shader management
  - GPU resource management

#### Success Criteria

- GPU rendering pipeline functional
- Shader system works correctly
- Performance is competitive

### 4.2 Threading & File I/O (v0.9.0)

**Status**: Not Started  
**Duration**: 6-8 days  
**Dependencies**: Core Systems

#### Tasks

- [ ] **Threading Utilities**
  - Thread-safe wrappers
  - Integration with std::thread
  - Synchronization primitives

- [ ] **File I/O System**
  - RAII file stream wrappers
  - Memory stream support
  - Path utilities

#### Success Criteria

- Threading utilities are safe and efficient
- File I/O operations work correctly

---

## Phase 5: Polish & Release (v1.0.0)

### 5.1 Documentation & Examples

**Status**: Not Started  
**Duration**: 10-14 days  
**Dependencies**: All Features

#### Tasks

- [ ] **API Documentation**
  - Doxygen configuration
  - Complete API documentation
  - Usage examples for all features

- [ ] **Tutorial Examples**
  - "Hello World" example
  - Basic game example
  - Advanced graphics example

- [ ] **Performance Documentation**
  - Benchmarking results
  - Performance comparison with raw SDL
  - Optimization guidelines

#### Success Criteria

- All APIs documented
- Examples compile and run
- Performance documentation complete

### 5.2 Packaging & Distribution

**Status**: Not Started  
**Duration**: 5-7 days  
**Dependencies**: Documentation & Examples

#### Tasks

- [ ] **CMake Package Configuration**
  - CMake Config files for find_package()
  - Version compatibility checking
  - Component-based installation

- [ ] **Package Manager Integration**
  - vcpkg port creation
  - Conan recipe creation
  - Package testing

- [ ] **Release Automation**
  - GitHub Releases automation
  - Binary artifact generation
  - Changelog generation

#### Success Criteria

- Package managers can install laya
- find_package() works correctly
- Release process is automated

---

## Risk Assessment & Mitigation

### High-Risk Items

#### 1. **SDL3 API Stability**

- **Risk**: SDL3 is still in development, API may change
- **Mitigation**:
  - Track SDL3 development closely
  - Design wrapper to be adaptable
  - Maintain compatibility layer if needed

#### 2. **C++20 Compiler Support**

- **Risk**: Not all target platforms may have full C++20 support
- **Mitigation**:
  - Test on multiple compiler versions
  - Provide fallbacks for missing features
  - Document minimum compiler requirements

#### 3. **Performance Overhead**

- **Risk**: Wrapper may introduce unacceptable overhead
- **Mitigation**:
  - Continuous performance benchmarking
  - Profile-guided optimization
  - Zero-overhead design principles

### Medium-Risk Items

#### 1. **Cross-Platform Compatibility**

- **Risk**: Platform-specific issues may arise
- **Mitigation**:
  - Comprehensive CI testing
  - Platform-specific testing
  - Community feedback integration

#### 2. **Memory Management Complexity**

- **Risk**: RAII wrappers may have subtle bugs
- **Mitigation**:
  - Extensive testing with sanitizers
  - Memory leak detection in CI
  - Code review for all RAII implementations

---

## Success Metrics

### Development Metrics

- **Code Coverage**: >90% for core functionality
- **Build Time**: <5 minutes for full build
- **Test Execution**: <30 seconds for full test suite

### Quality Metrics

- **Memory Leaks**: Zero in test suite
- **Performance Overhead**: <5% vs raw SDL
- **API Completeness**: >95% SDL3 coverage

### Community Metrics

- **Documentation Coverage**: 100% public API
- **Example Coverage**: All major use cases
- **Issue Response Time**: <48 hours

---

## Timeline Summary

```
Phase 0 (Foundation):     2-3 weeks
Phase 1 (Core Systems):   6-8 weeks  
Phase 2 (Graphics/Media): 3-4 weeks
Phase 3 (Input Systems):  2-3 weeks
Phase 4 (Advanced):       4-5 weeks
Phase 5 (Polish):         3-4 weeks
Total Estimated Time:     20-27 weeks (5-7 months)
```

### Milestone Schedule

- **v0.1.0** (Init/Error): Week 4
- **v0.2.0** (Windows): Week 6
- **v0.3.0** (Rendering): Week 9
- **v0.4.0** (Events): Week 12
- **v0.5.0** (Textures): Week 15
- **v0.6.0** (Audio): Week 17
- **v0.7.0** (Input): Week 19
- **v0.8.0** (GPU): Week 22
- **v0.9.0** (Threading): Week 24
- **v1.0.0** (Release): Week 27

This timeline assumes a single full-time developer. With multiple contributors, phases can be parallelized to reduce overall timeline.
