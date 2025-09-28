# Initialization Subsystem Implementation Plan

This document provides a detailed implementation plan for the SDL3 initialization and shutdown subsystem wrapper in laya.

---

## Overview

The initialization subsystem is the foundation of laya, responsible for:

- Safe SDL3 initialization and shutdown
- Subsystem management and dependency tracking
- Global state management
- Error handling infrastructure

**Target Version**: v0.1.0  
**Priority**: Critical  
**Estimated Effort**: 5-7 days

---

## Design Goals

### 1. **RAII-Based Lifecycle Management**

Ensure SDL3 is properly initialized and cleaned up automatically, preventing resource leaks and initialization errors.

### 2. **Type-Safe Subsystem Management**

Replace SDL3's integer flags with type-safe enum classes and provide dependency validation.

### 3. **Exception Safety**

Guarantee that initialization failures are properly handled and don't leave SDL3 in an inconsistent state.

### 4. **Thread Safety**

Ensure initialization can be safely called from multiple threads (though SDL3 itself may have limitations).

---

## API Design

### Core Types

```cpp
namespace laya {
    // Subsystem enumeration
    enum class subsystem : uint32_t {
        timer       = SDL_INIT_TIMER,
        audio       = SDL_INIT_AUDIO,
        video       = SDL_INIT_VIDEO,
        joystick    = SDL_INIT_JOYSTICK,
        haptic      = SDL_INIT_HAPTIC,
        gamepad     = SDL_INIT_GAMEPAD,
        events      = SDL_INIT_EVENTS,
        sensor      = SDL_INIT_SENSOR,
        camera      = SDL_INIT_CAMERA
    };
    
    // Enable bitwise operations for subsystem flags
    constexpr subsystem operator|(subsystem lhs, subsystem rhs) noexcept;
    constexpr subsystem operator&(subsystem lhs, subsystem rhs) noexcept;
    constexpr subsystem operator^(subsystem lhs, subsystem rhs) noexcept;
    constexpr subsystem operator~(subsystem flags) noexcept;
    
    // RAII context class
    class context {
    public:
        // Constructor - initializes SDL3 with specified subsystems
        explicit context(subsystem systems = subsystem::video);
        explicit context(std::initializer_list<subsystem> systems);
        
        // Destructor - ensures proper SDL3 shutdown
        ~context();
        
        // Non-copyable, movable
        context(const context&) = delete;
        context& operator=(const context&) = delete;
        context(context&& other) noexcept;
        context& operator=(context&& other) noexcept;
        
        // Subsystem management
        void init_subsystem(subsystem sys);
        void quit_subsystem(subsystem sys);
        bool is_initialized(subsystem sys) const;
        subsystem initialized_subsystems() const;
        
        // Global state queries
        static bool is_any_initialized();
        static std::string version_string();
        static version_info version();
    };
    
    // Version information
    struct version_info {
        int major, minor, patch;
        
        constexpr bool operator==(const version_info& other) const noexcept;
        constexpr bool operator!=(const version_info& other) const noexcept;
        constexpr bool operator<(const version_info& other) const noexcept;
        constexpr bool operator<=(const version_info& other) const noexcept;
        constexpr bool operator>(const version_info& other) const noexcept;
        constexpr bool operator>=(const version_info& other) const noexcept;
    };
    
    // Convenience functions
    void init(subsystem systems = subsystem::video);
    void init(std::initializer_list<subsystem> systems);
    void quit();
    
    bool is_initialized(subsystem sys);
    subsystem initialized_subsystems();
}
```

---

## Implementation Details

### 1. **Subsystem Enum Implementation**

```cpp
// include/laya/core/subsystem.hpp
#pragma once

#include <SDL3/SDL.h>
#include <cstdint>

namespace laya {
    enum class subsystem : uint32_t {
        timer       = SDL_INIT_TIMER,
        audio       = SDL_INIT_AUDIO,
        video       = SDL_INIT_VIDEO,
        joystick    = SDL_INIT_JOYSTICK,
        haptic      = SDL_INIT_HAPTIC,
        gamepad     = SDL_INIT_GAMEPAD,
        events      = SDL_INIT_EVENTS,
        sensor      = SDL_INIT_SENSOR,
        camera      = SDL_INIT_CAMERA
    };
    
    // Bitwise operators
    constexpr subsystem operator|(subsystem lhs, subsystem rhs) noexcept {
        return static_cast<subsystem>(
            static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
        );
    }
    
    constexpr subsystem operator&(subsystem lhs, subsystem rhs) noexcept {
        return static_cast<subsystem>(
            static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
        );
    }
    
    constexpr subsystem operator^(subsystem lhs, subsystem rhs) noexcept {
        return static_cast<subsystem>(
            static_cast<uint32_t>(lhs) ^ static_cast<uint32_t>(rhs)
        );
    }
    
    constexpr subsystem operator~(subsystem flags) noexcept {
        return static_cast<subsystem>(~static_cast<uint32_t>(flags));
    }
    
    // Compound assignment operators
    constexpr subsystem& operator|=(subsystem& lhs, subsystem rhs) noexcept {
        return lhs = lhs | rhs;
    }
    
    constexpr subsystem& operator&=(subsystem& lhs, subsystem rhs) noexcept {
        return lhs = lhs & rhs;
    }
    
    constexpr subsystem& operator^=(subsystem& lhs, subsystem rhs) noexcept {
        return lhs = lhs ^ rhs;
    }
    
    // Utility functions
    constexpr bool has_subsystem(subsystem flags, subsystem test) noexcept {
        return (flags & test) == test;
    }
    
    constexpr uint32_t to_sdl_flags(subsystem sys) noexcept {
        return static_cast<uint32_t>(sys);
    }
    
    constexpr subsystem from_sdl_flags(uint32_t flags) noexcept {
        return static_cast<subsystem>(flags);
    }
}
```

### 2. **Context Class Implementation**

```cpp
// include/laya/core/context.hpp
#pragma once

#include "subsystem.hpp"
#include "error.hpp"
#include <initializer_list>
#include <mutex>
#include <atomic>

namespace laya {
    struct version_info {
        int major, minor, patch;
        
        constexpr bool operator==(const version_info& other) const noexcept {
            return major == other.major && minor == other.minor && patch == other.patch;
        }
        
        constexpr bool operator!=(const version_info& other) const noexcept {
            return !(*this == other);
        }
        
        constexpr bool operator<(const version_info& other) const noexcept {
            if (major != other.major) return major < other.major;
            if (minor != other.minor) return minor < other.minor;
            return patch < other.patch;
        }
        
        constexpr bool operator<=(const version_info& other) const noexcept {
            return *this < other || *this == other;
        }
        
        constexpr bool operator>(const version_info& other) const noexcept {
            return !(*this <= other);
        }
        
        constexpr bool operator>=(const version_info& other) const noexcept {
            return !(*this < other);
        }
    };
    
    class context {
    private:
        subsystem m_initialized_systems{};
        static std::atomic<int> s_instance_count;
        static std::mutex s_init_mutex;
        
    public:
        explicit context(subsystem systems = subsystem::video);
        explicit context(std::initializer_list<subsystem> systems);
        ~context();
        
        // Non-copyable, movable
        context(const context&) = delete;
        context& operator=(const context&) = delete;
        context(context&& other) noexcept;
        context& operator=(context&& other) noexcept;
        
        // Subsystem management
        void init_subsystem(subsystem sys);
        void quit_subsystem(subsystem sys);
        bool is_initialized(subsystem sys) const noexcept;
        subsystem initialized_subsystems() const noexcept;
        
        // Global state queries
        static bool is_any_initialized() noexcept;
        static std::string version_string();
        static version_info version();
        
    private:
        void init_impl(subsystem systems);
        void quit_impl() noexcept;
        static subsystem combine_subsystems(std::initializer_list<subsystem> systems);
    };
}
```

### 3. **Context Implementation**

```cpp
// src/core/context.cpp
#include "laya/core/context.hpp"
#include <SDL3/SDL.h>
#include <stdexcept>
#include <lock_guard>

namespace laya {
    std::atomic<int> context::s_instance_count{0};
    std::mutex context::s_init_mutex;
    
    context::context(subsystem systems) {
        init_impl(systems);
    }
    
    context::context(std::initializer_list<subsystem> systems) {
        init_impl(combine_subsystems(systems));
    }
    
    context::~context() {
        quit_impl();
    }
    
    context::context(context&& other) noexcept 
        : m_initialized_systems(std::exchange(other.m_initialized_systems, subsystem{})) {
        // Transfer ownership - other context no longer owns the initialization
    }
    
    context& context::operator=(context&& other) noexcept {
        if (this != &other) {
            quit_impl();  // Clean up current state
            m_initialized_systems = std::exchange(other.m_initialized_systems, subsystem{});
        }
        return *this;
    }
    
    void context::init_subsystem(subsystem sys) {
        std::lock_guard<std::mutex> lock(s_init_mutex);
        
        if (SDL_InitSubSystem(to_sdl_flags(sys)) < 0) {
            throw error::from_sdl();
        }
        
        m_initialized_systems |= sys;
    }
    
    void context::quit_subsystem(subsystem sys) {
        std::lock_guard<std::mutex> lock(s_init_mutex);
        
        if (has_subsystem(m_initialized_systems, sys)) {
            SDL_QuitSubSystem(to_sdl_flags(sys));
            m_initialized_systems &= ~sys;
        }
    }
    
    bool context::is_initialized(subsystem sys) const noexcept {
        return has_subsystem(m_initialized_systems, sys);
    }
    
    subsystem context::initialized_subsystems() const noexcept {
        return m_initialized_systems;
    }
    
    bool context::is_any_initialized() noexcept {
        return s_instance_count.load() > 0;
    }
    
    std::string context::version_string() {
        return SDL_GetVersion();
    }
    
    version_info context::version() {
        int major, minor, patch;
        SDL_GetVersion(&major, &minor, &patch);
        return {major, minor, patch};
    }
    
    void context::init_impl(subsystem systems) {
        std::lock_guard<std::mutex> lock(s_init_mutex);
        
        // First context initializes SDL
        if (s_instance_count.fetch_add(1) == 0) {
            if (SDL_Init(to_sdl_flags(systems)) < 0) {
                s_instance_count.fetch_sub(1);
                throw error::from_sdl();
            }
        } else {
            // Additional contexts just initialize their subsystems
            if (SDL_InitSubSystem(to_sdl_flags(systems)) < 0) {
                s_instance_count.fetch_sub(1);
                throw error::from_sdl();
            }
        }
        
        m_initialized_systems = systems;
    }
    
    void context::quit_impl() noexcept {
        if (m_initialized_systems != subsystem{}) {
            std::lock_guard<std::mutex> lock(s_init_mutex);
            
            // Quit our subsystems
            SDL_QuitSubSystem(to_sdl_flags(m_initialized_systems));
            
            // Last context quits SDL entirely
            if (s_instance_count.fetch_sub(1) == 1) {
                SDL_Quit();
            }
            
            m_initialized_systems = subsystem{};
        }
    }
    
    subsystem context::combine_subsystems(std::initializer_list<subsystem> systems) {
        subsystem result{};
        for (auto sys : systems) {
            result |= sys;
        }
        return result;
    }
    
    // Convenience functions
    void init(subsystem systems) {
        static context global_context(systems);
    }
    
    void init(std::initializer_list<subsystem> systems) {
        static context global_context(systems);
    }
    
    void quit() {
        // Global context will be destroyed at program exit
        // This function exists for API compatibility but doesn't do anything
        // in the RAII design
    }
    
    bool is_initialized(subsystem sys) {
        return SDL_WasInit(to_sdl_flags(sys)) != 0;
    }
    
    subsystem initialized_subsystems() {
        return from_sdl_flags(SDL_WasInit(0));
    }
}
```

---

## Error Handling Integration

### Error Class Design

```cpp
// include/laya/core/error.hpp
#pragma once

#include <stdexcept>
#include <string>

namespace laya {
    class error : public std::runtime_error {
    public:
        explicit error(const std::string& message);
        explicit error(const char* message);
        
        // Create error from current SDL error state
        static error from_sdl();
        
        // Create error with custom message and SDL error
        static error from_sdl(const std::string& context);
        
    private:
        static std::string get_sdl_error();
    };
}
```

```cpp
// src/core/error.cpp
#include "laya/core/error.hpp"
#include <SDL3/SDL.h>

namespace laya {
    error::error(const std::string& message) : std::runtime_error(message) {}
    
    error::error(const char* message) : std::runtime_error(message) {}
    
    error error::from_sdl() {
        std::string sdl_error = get_sdl_error();
        return error(sdl_error.empty() ? "Unknown SDL error" : sdl_error);
    }
    
    error error::from_sdl(const std::string& context) {
        std::string sdl_error = get_sdl_error();
        if (sdl_error.empty()) {
            return error(context + ": Unknown SDL error");
        } else {
            return error(context + ": " + sdl_error);
        }
    }
    
    std::string error::get_sdl_error() {
        const char* sdl_error = SDL_GetError();
        if (sdl_error && *sdl_error) {
            std::string result(sdl_error);
            SDL_ClearError();  // Clear the error after reading
            return result;
        }
        return {};
    }
}
```

---

## Testing Strategy

### Unit Tests

```cpp
// tests/unit/core/test_context.cpp
#include <doctest/doctest.h>
#include "laya/core/context.hpp"
#include "laya/core/error.hpp"

TEST_SUITE("Context") {
    TEST_CASE("Default construction initializes video subsystem") {
        laya::context ctx;
        CHECK(ctx.is_initialized(laya::subsystem::video));
        CHECK(laya::context::is_any_initialized());
    }
    
    TEST_CASE("Explicit subsystem initialization") {
        laya::context ctx(laya::subsystem::audio | laya::subsystem::video);
        CHECK(ctx.is_initialized(laya::subsystem::audio));
        CHECK(ctx.is_initialized(laya::subsystem::video));
        CHECK_FALSE(ctx.is_initialized(laya::subsystem::joystick));
    }
    
    TEST_CASE("Initializer list construction") {
        laya::context ctx{laya::subsystem::audio, laya::subsystem::events};
        CHECK(ctx.is_initialized(laya::subsystem::audio));
        CHECK(ctx.is_initialized(laya::subsystem::events));
        CHECK_FALSE(ctx.is_initialized(laya::subsystem::video));
    }
    
    TEST_CASE("Move semantics") {
        laya::context ctx1(laya::subsystem::video);
        CHECK(ctx1.is_initialized(laya::subsystem::video));
        
        laya::context ctx2 = std::move(ctx1);
        CHECK(ctx2.is_initialized(laya::subsystem::video));
        CHECK_FALSE(ctx1.is_initialized(laya::subsystem::video));
    }
    
    TEST_CASE("Runtime subsystem management") {
        laya::context ctx(laya::subsystem::video);
        
        ctx.init_subsystem(laya::subsystem::audio);
        CHECK(ctx.is_initialized(laya::subsystem::audio));
        
        ctx.quit_subsystem(laya::subsystem::audio);
        CHECK_FALSE(ctx.is_initialized(laya::subsystem::audio));
    }
    
    TEST_CASE("Multiple contexts") {
        {
            laya::context ctx1(laya::subsystem::video);
            CHECK(laya::context::is_any_initialized());
            
            {
                laya::context ctx2(laya::subsystem::audio);
                CHECK(ctx2.is_initialized(laya::subsystem::audio));
            }
            
            CHECK(laya::context::is_any_initialized());
        }
        
        // After all contexts destroyed, SDL should be quit
        // Note: This test may be flaky due to static initialization order
    }
    
    TEST_CASE("Version information") {
        auto version = laya::context::version();
        CHECK(version.major >= 3);  // SDL3
        
        auto version_str = laya::context::version_string();
        CHECK_FALSE(version_str.empty());
    }
}

TEST_SUITE("Subsystem enum") {
    TEST_CASE("Bitwise operations") {
        auto combined = laya::subsystem::video | laya::subsystem::audio;
        CHECK(laya::has_subsystem(combined, laya::subsystem::video));
        CHECK(laya::has_subsystem(combined, laya::subsystem::audio));
        CHECK_FALSE(laya::has_subsystem(combined, laya::subsystem::joystick));
        
        auto masked = combined & laya::subsystem::video;
        CHECK(masked == laya::subsystem::video);
    }
    
    TEST_CASE("SDL flag conversion") {
        CHECK(laya::to_sdl_flags(laya::subsystem::video) == SDL_INIT_VIDEO);
        CHECK(laya::from_sdl_flags(SDL_INIT_AUDIO) == laya::subsystem::audio);
    }
}
```

### Integration Tests

```cpp
// tests/integration/test_init_integration.cpp
#include <doctest/doctest.h>
#include "laya/core/context.hpp"
#include <thread>
#include <vector>

TEST_SUITE("Initialization Integration") {
    TEST_CASE("Thread safety") {
        std::vector<std::thread> threads;
        std::atomic<int> success_count{0};
        
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&]() {
                try {
                    laya::context ctx(laya::subsystem::video);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    success_count.fetch_add(1);
                } catch (...) {
                    // Thread failed to initialize
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        CHECK(success_count.load() == 10);
    }
    
    TEST_CASE("Exception safety") {
        // This test would need to mock SDL_Init to return failure
        // to test exception safety properly
        SUBCASE("Invalid subsystem handling") {
            // Test with invalid subsystem combinations if any exist
        }
    }
}
```

---

## Performance Considerations

### 1. **Initialization Overhead**

- Context creation should be fast (< 1ms on modern systems)
- Subsystem initialization times depend on SDL3 implementation
- Multiple context creation should have minimal overhead

### 2. **Memory Usage**

- Context class should be lightweight (< 64 bytes)
- No dynamic allocations in normal operation
- Static data should be minimal

### 3. **Thread Safety**

- Mutex usage only during initialization/shutdown
- No locking for read-only operations
- Atomic operations for reference counting

---

## Implementation Checklist

### Phase 1: Basic Structure

- [ ] Create subsystem enum with bitwise operators
- [ ] Implement version_info struct with comparison operators
- [ ] Create basic context class structure
- [ ] Add error handling integration

### Phase 2: Core Functionality

- [ ] Implement context constructor/destructor
- [ ] Add move semantics support
- [ ] Implement subsystem management methods
- [ ] Add thread safety with mutex

### Phase 3: Convenience Features

- [ ] Add convenience init/quit functions
- [ ] Implement global state queries
- [ ] Add initializer list support
- [ ] Create utility functions

### Phase 4: Testing & Polish

- [ ] Write comprehensive unit tests
- [ ] Add integration tests
- [ ] Performance benchmarking
- [ ] Documentation and examples

### Phase 5: Validation

- [ ] Memory leak testing with valgrind
- [ ] Thread safety testing
- [ ] Exception safety validation
- [ ] Cross-platform testing

---

## Dependencies

### Build Dependencies

- SDL3 development headers
- C++20 compatible compiler
- CMake 3.20+

### Runtime Dependencies

- SDL3 shared library
- Platform-specific system libraries

### Testing Dependencies

- doctest testing framework
- Threading support
- Benchmarking tools

---

## Risk Mitigation

### 1. **SDL3 API Changes**

- Monitor SDL3 development for API changes
- Use SDL3 version detection in CMake
- Maintain compatibility layer if needed

### 2. **Thread Safety Issues**

- Extensive testing with thread sanitizer
- Clear documentation of thread safety guarantees
- Conservative locking strategy

### 3. **Performance Regression**

- Benchmark against raw SDL3 initialization
- Profile memory usage and allocation patterns
- Optimize hot paths identified by profiling

This implementation plan provides a solid foundation for the laya initialization subsystem, ensuring safety, performance, and maintainability while preserving SDL3's capabilities.
