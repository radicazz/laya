# Features

This document provides a comprehensive mapping of SDL3 features to laya wrapper components, including priority levels and version targeting.

---

## Feature Classification

### Priority Levels

- **🔴 Critical**: Essential for basic functionality, required for v0.1.0-v0.3.0
- **🟡 Important**: Significant features needed for most applications, v0.4.0-v0.7.0
- **🟢 Nice-to-Have**: Advanced or specialized features, v0.8.0-v1.0.0
- **🔵 Future**: Post-1.0 features or SDL3 extensions

### Complexity Ratings

- **Simple**: Straightforward wrapper, minimal design decisions
- **Moderate**: Some design challenges, multiple implementation approaches
- **Complex**: Significant architectural decisions, extensive testing needed

---

## Core Systems

### 1. Initialization & Shutdown 🔴

**Target Version**: v0.1.0  
**Complexity**: Simple

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_Init()` | `laya::init()` | RAII initialization guard |
| `SDL_InitSubSystem()` | `laya::init_subsystem()` | Type-safe subsystem enum |
| `SDL_Quit()` | `laya::quit()` | Automatic via RAII destructor |
| `SDL_QuitSubSystem()` | `laya::quit_subsystem()` | Type-safe subsystem enum |
| `SDL_WasInit()` | `laya::is_initialized()` | Returns subsystem status |

**Design Considerations**:

- Global initialization state management
- Exception safety during initialization
- Subsystem dependency tracking

```cpp
// Proposed API
namespace laya {
    enum class subsystem {
        timer, audio, video, joystick, haptic, gamepad, events, sensor, camera
    };
    
    class context {
    public:
        explicit context(std::initializer_list<subsystem> systems = {subsystem::video});
        ~context();
        // Non-copyable, movable
    };
    
    // Convenience functions
    void init(std::initializer_list<subsystem> systems = {subsystem::video});
    void quit();
}
```

---

### 2. Error Handling 🔴

**Target Version**: v0.1.0  
**Complexity**: Moderate

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_GetError()` | `laya::get_last_error()` | Returns `std::string` |
| `SDL_SetError()` | `laya::set_error()` | For library extensions |
| `SDL_ClearError()` | `laya::clear_error()` | Automatic in most cases |

**Design Considerations**:

- Exception vs error code strategy
- Integration with `std::expected` (C++23) or custom result type
- Thread-local error state handling

```cpp
// Proposed API
namespace laya {
    template<typename T>
    using result = std::expected<T, std::string>;  // or custom error type
    
    class error : public std::runtime_error {
    public:
        explicit error(const std::string& msg);
        static error from_sdl();
    };
    
    std::string get_last_error();
    void clear_error();
}
```

---

### 3. Window Management 🔴

**Target Version**: v0.2.0  
**Complexity**: Moderate

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_CreateWindow()` | `laya::window` constructor | RAII resource management |
| `SDL_DestroyWindow()` | `laya::window` destructor | Automatic cleanup |
| `SDL_SetWindowTitle()` | `window::set_title()` | String parameter |
| `SDL_GetWindowTitle()` | `window::title()` | Returns `std::string_view` |
| `SDL_SetWindowSize()` | `window::set_size()` | Type-safe size struct |
| `SDL_GetWindowSize()` | `window::size()` | Returns size struct |
| `SDL_SetWindowPosition()` | `window::set_position()` | Type-safe position struct |
| `SDL_GetWindowPosition()` | `window::position()` | Returns position struct |
| `SDL_ShowWindow()` | `window::show()` | |
| `SDL_HideWindow()` | `window::hide()` | |
| `SDL_MaximizeWindow()` | `window::maximize()` | |
| `SDL_MinimizeWindow()` | `window::minimize()` | |
| `SDL_RestoreWindow()` | `window::restore()` | |

**Design Considerations**:

- Window flags as type-safe enum class
- Position and size as strong types
- Window state management
- Multi-monitor support

```cpp
// Proposed API
namespace laya {
    enum class window_flags : uint32_t {
        none = 0,
        fullscreen = SDL_WINDOW_FULLSCREEN,
        resizable = SDL_WINDOW_RESIZABLE,
        borderless = SDL_WINDOW_BORDERLESS,
        // ... other flags
    };
    
    struct size { int width, height; };
    struct position { int x, y; };
    
    class window {
    public:
        window(std::string_view title, size sz, window_flags flags = window_flags::none);
        ~window();
        
        // Non-copyable, movable
        window(const window&) = delete;
        window& operator=(const window&) = delete;
        window(window&&) noexcept;
        window& operator=(window&&) noexcept;
        
        void set_title(std::string_view title);
        std::string_view title() const;
        
        void set_size(size sz);
        size size() const;
        
        void show();
        void hide();
        // ... other methods
        
        SDL_Window* native_handle() const;  // For interop
    };
}
```

---

### 4. Rendering (2D) 🔴

**Target Version**: v0.3.0  
**Complexity**: Complex

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_CreateRenderer()` | `laya::renderer` constructor | RAII resource management |
| `SDL_DestroyRenderer()` | `laya::renderer` destructor | Automatic cleanup |
| `SDL_RenderClear()` | `renderer::clear()` | Optional color parameter |
| `SDL_RenderPresent()` | `renderer::present()` | |
| `SDL_SetRenderDrawColor()` | `renderer::set_draw_color()` | Type-safe color struct |
| `SDL_RenderPoint()` | `renderer::draw_point()` | Type-safe point struct |
| `SDL_RenderLine()` | `renderer::draw_line()` | Type-safe line struct |
| `SDL_RenderRect()` | `renderer::draw_rect()` | Type-safe rect struct |
| `SDL_RenderFillRect()` | `renderer::fill_rect()` | Type-safe rect struct |

**Design Considerations**:

- Color representation (RGB, RGBA, HSV options)
- Geometric primitives as strong types
- Texture management integration
- Render target support

```cpp
// Proposed API
namespace laya {
    struct color {
        uint8_t r, g, b, a = 255;
        
        static constexpr color white() { return {255, 255, 255}; }
        static constexpr color black() { return {0, 0, 0}; }
        // ... other common colors
    };
    
    struct point { int x, y; };
    struct rect { int x, y, int width, height; };
    
    class renderer {
    public:
        explicit renderer(const window& win);
        ~renderer();
        
        void clear(color c = color::black());
        void present();
        
        void set_draw_color(color c);
        color draw_color() const;
        
        void draw_point(point p);
        void draw_line(point start, point end);
        void draw_rect(rect r);
        void fill_rect(rect r);
        
        SDL_Renderer* native_handle() const;
    };
}
```

---

### 5. Event Handling 🔴

**Target Version**: v0.4.0  
**Complexity**: Complex

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_PollEvent()` | `laya::poll_events()` | Returns range/iterator |
| `SDL_WaitEvent()` | `laya::wait_event()` | Returns optional event |
| `SDL_PushEvent()` | `laya::push_event()` | Type-safe event creation |

**Design Considerations**:

- Type-safe event variant system
- Range-based event polling
- Event filtering and handling
- Custom event types

```cpp
// Proposed API
namespace laya {
    struct quit_event {};
    
    struct key_event {
        enum class state { pressed, released };
        state key_state;
        // ... key details
    };
    
    struct mouse_button_event {
        enum class state { pressed, released };
        enum class button { left, right, middle };
        state button_state;
        button mouse_button;
        point position;
    };
    
    using event = std::variant<
        quit_event,
        key_event,
        mouse_button_event
        // ... other event types
    >;
    
    class event_range {
        // Iterator implementation for range-based for loops
    };
    
    event_range poll_events();
    std::optional<event> wait_event();
    std::optional<event> wait_event_timeout(std::chrono::milliseconds timeout);
}
```

---

## Graphics & Media

### 6. Textures 🟡

**Target Version**: v0.5.0  
**Complexity**: Moderate

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_CreateTexture()` | `laya::texture` constructor | RAII resource management |
| `SDL_CreateTextureFromSurface()` | `texture::from_surface()` | Factory function |
| `SDL_DestroyTexture()` | `laya::texture` destructor | Automatic cleanup |
| `SDL_RenderTexture()` | `renderer::draw_texture()` | Overloaded variants |

### 7. Surfaces 🟡

**Target Version**: v0.5.0  
**Complexity**: Moderate

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_CreateSurface()` | `laya::surface` constructor | RAII resource management |
| `SDL_LoadBMP()` | `surface::load_bmp()` | Factory function |
| `SDL_SaveBMP()` | `surface::save_bmp()` | Member function |

### 8. Audio 🟡

**Target Version**: v0.6.0  
**Complexity**: Complex

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_OpenAudioDevice()` | `laya::audio_device` | RAII resource management |
| `SDL_CloseAudioDevice()` | `audio_device` destructor | Automatic cleanup |
| `SDL_PlayAudioDevice()` | `audio_device::play()` | |
| `SDL_PauseAudioDevice()` | `audio_device::pause()` | |

---

## Input Systems

### 9. Keyboard Input 🟡

**Target Version**: v0.4.0  
**Complexity**: Simple

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_GetKeyboardState()` | `laya::keyboard::state()` | Returns keyboard state object |
| `SDL_GetModState()` | `laya::keyboard::modifiers()` | Type-safe modifier flags |

### 10. Mouse Input 🟡

**Target Version**: v0.4.0  
**Complexity**: Simple

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_GetMouseState()` | `laya::mouse::state()` | Returns mouse state object |
| `SDL_SetCursor()` | `laya::mouse::set_cursor()` | Type-safe cursor management |

### 11. Gamepad Input 🟡

**Target Version**: v0.7.0  
**Complexity**: Moderate

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_OpenGamepad()` | `laya::gamepad` constructor | RAII resource management |
| `SDL_CloseGamepad()` | `gamepad` destructor | Automatic cleanup |
| `SDL_GetGamepadButton()` | `gamepad::button()` | Type-safe button enum |

---

## Advanced Features

### 12. GPU Rendering (New in SDL3) 🟢

**Target Version**: v0.8.0  
**Complexity**: Complex

SDL3 introduces a new GPU API for modern graphics programming. This will require extensive design work to create a safe C++ interface.

### 13. Threading 🟢

**Target Version**: v0.8.0  
**Complexity**: Moderate

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_CreateThread()` | `laya::thread` | Integrate with `std::thread` |
| `SDL_CreateMutex()` | `laya::mutex` | Integrate with `std::mutex` |

### 14. File I/O 🟢

**Target Version**: v0.9.0  
**Complexity**: Simple

| SDL3 Function | laya Wrapper | Notes |
|---------------|--------------|-------|
| `SDL_RWFromFile()` | `laya::file_stream` | RAII file handling |
| `SDL_RWFromMem()` | `laya::memory_stream` | Memory buffer wrapper |

---

## Extension Libraries

### 15. SDL_image Support 🔵

**Target Version**: v1.1.0  
**Complexity**: Moderate

Optional wrapper for SDL_image functionality:

- PNG, JPEG, WEBP loading
- Type-safe format detection
- Integration with laya::surface and laya::texture

### 16. SDL_ttf Support 🔵

**Target Version**: v1.2.0  
**Complexity**: Moderate

Optional wrapper for SDL_ttf functionality:

- Font loading and management
- Text rendering to surfaces/textures
- Unicode support

### 17. SDL_mixer Support 🔵

**Target Version**: v1.3.0  
**Complexity**: Complex

Optional wrapper for SDL_mixer functionality:

- Audio mixing and effects
- Music and sound effect playback
- 3D audio positioning

---

## Implementation Priority Matrix

```
High Priority (v0.1-v0.4):
┌─────────────────┬─────────┬──────────┐
│ Feature         │ Version │ Blocking │
├─────────────────┼─────────┼──────────┤
│ Init/Shutdown   │ v0.1.0  │ None     │
│ Error Handling  │ v0.1.0  │ None     │
│ Window Mgmt     │ v0.2.0  │ Init     │
│ 2D Rendering    │ v0.3.0  │ Window   │
│ Event Handling  │ v0.4.0  │ Window   │
└─────────────────┴─────────┴──────────┘

Medium Priority (v0.5-v0.7):
┌─────────────────┬─────────┬──────────┐
│ Feature         │ Version │ Blocking │
├─────────────────┼─────────┼──────────┤
│ Textures        │ v0.5.0  │ Render   │
│ Surfaces        │ v0.5.0  │ Render   │
│ Audio           │ v0.6.0  │ Init     │
│ Gamepad         │ v0.7.0  │ Events   │
└─────────────────┴─────────┴──────────┘

Low Priority (v0.8-v1.0):
┌─────────────────┬─────────┬──────────┐
│ Feature         │ Version │ Blocking │
├─────────────────┼─────────┼──────────┤
│ GPU Rendering   │ v0.8.0  │ Window   │
│ Threading       │ v0.8.0  │ None     │
│ File I/O        │ v0.9.0  │ None     │
│ Polish & Docs   │ v1.0.0  │ All      │
└─────────────────┴─────────┴──────────┘
```

---

## Feature Completeness Goals

### v0.5.0 (Alpha) - 40% SDL3 Coverage

- Basic window and rendering
- Essential event handling
- Core resource management

### v0.8.0 (Beta) - 80% SDL3 Coverage

- Complete 2D graphics pipeline
- Full input system support
- Audio system integration

### v1.0.0 (Stable) - 95% SDL3 Coverage

- All commonly used SDL3 features
- Extension library support
- Production-ready stability

### Post-1.0 - 100% SDL3 Coverage

- Specialized and platform-specific features
- Advanced GPU rendering capabilities
- Complete feature parity with SDL3
