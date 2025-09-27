# Wrapping Techniques

This document outlines the C++20 patterns, techniques, and design principles used to create safe, efficient wrappers around SDL3's C API.

---

## Core Principles

### 1. **RAII (Resource Acquisition Is Initialization)**

Every SDL resource must be wrapped in a C++ class that automatically manages its lifetime.

```cpp
// Bad: Manual resource management
SDL_Window* window = SDL_CreateWindow("Title", 800, 600, 0);
// ... use window
SDL_DestroyWindow(window);  // Easy to forget or skip on error paths

// Good: RAII wrapper
class window {
    SDL_Window* m_window;
public:
    window(std::string_view title, size sz, window_flags flags = window_flags::none)
        : m_window(SDL_CreateWindow(title.data(), sz.width, sz.height, static_cast<uint32_t>(flags)))
    {
        if (!m_window) {
            throw error::from_sdl();
        }
    }
    
    ~window() {
        if (m_window) {
            SDL_DestroyWindow(m_window);
        }
    }
    
    // Non-copyable, movable
    window(const window&) = delete;
    window& operator=(const window&) = delete;
    
    window(window&& other) noexcept : m_window(std::exchange(other.m_window, nullptr)) {}
    window& operator=(window&& other) noexcept {
        if (this != &other) {
            if (m_window) SDL_DestroyWindow(m_window);
            m_window = std::exchange(other.m_window, nullptr);
        }
        return *this;
    }
};
```

### 2. **Strong Type Safety**

Replace SDL's loose typing with C++ strong types to prevent logical errors at compile time.

```cpp
// Bad: Easy to mix up parameters
SDL_SetWindowSize(window, 600, 800);  // Oops, swapped width/height

// Good: Strong types prevent errors
struct size { int width, height; };
struct position { int x, y; };

void window::set_size(size sz) {
    SDL_SetWindowSize(m_window, sz.width, sz.height);
}

void window::set_position(position pos) {
    SDL_SetWindowPosition(m_window, pos.x, pos.y);
}

// Usage - impossible to mix up
win.set_size({800, 600});
win.set_position({100, 50});
```

### 3. **Enum Class for Constants**

Replace SDL's C-style defines with type-safe enum classes.

```cpp
// Bad: C-style defines, no type safety
SDL_CreateWindow("Title", 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);

// Good: Type-safe enum class with bitwise operations
enum class window_flags : uint32_t {
    none = 0,
    fullscreen = SDL_WINDOW_FULLSCREEN,
    resizable = SDL_WINDOW_RESIZABLE,
    borderless = SDL_WINDOW_BORDERLESS,
    always_on_top = SDL_WINDOW_ALWAYS_ON_TOP,
    // ... other flags
};

// Enable bitwise operations
constexpr window_flags operator|(window_flags lhs, window_flags rhs) {
    return static_cast<window_flags>(
        static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
    );
}

// Usage
auto flags = window_flags::resizable | window_flags::borderless;
window win("Title", {800, 600}, flags);
```

---

## Error Handling Strategies

### 1. **Exception-Based Error Handling**

Convert SDL's error codes into C++ exceptions for automatic error propagation.

```cpp
class error : public std::runtime_error {
public:
    explicit error(const std::string& msg) : std::runtime_error(msg) {}
    
    static error from_sdl() {
        const char* sdl_error = SDL_GetError();
        return error(sdl_error ? sdl_error : "Unknown SDL error");
    }
};

// Helper macro for SDL function calls
#define LAYA_SDL_CALL(func, ...) \
    do { \
        if (func(__VA_ARGS__) < 0) { \
            throw error::from_sdl(); \
        } \
    } while(0)

// Usage
void renderer::clear(color c) {
    LAYA_SDL_CALL(SDL_SetRenderDrawColor, m_renderer, c.r, c.g, c.b, c.a);
    LAYA_SDL_CALL(SDL_RenderClear, m_renderer);
}
```

### 2. **std::expected for Non-Exceptional Errors**

For operations where failure is expected, use `std::expected` (C++23) or a custom result type.

```cpp
template<typename T, typename E = std::string>
class result {
    std::variant<T, E> m_value;
    
public:
    result(T value) : m_value(std::move(value)) {}
    result(E error) : m_value(std::move(error)) {}
    
    bool has_value() const { return std::holds_alternative<T>(m_value); }
    const T& value() const { return std::get<T>(m_value); }
    const E& error() const { return std::get<E>(m_value); }
    
    explicit operator bool() const { return has_value(); }
};

// Usage for operations that might fail
result<texture> texture::load_from_file(const std::filesystem::path& path) {
    SDL_Surface* surface = IMG_Load(path.string().c_str());
    if (!surface) {
        return std::string(SDL_GetError());
    }
    
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!tex) {
        return std::string(SDL_GetError());
    }
    
    return texture(tex);  // RAII wrapper takes ownership
}
```

---

## Template Metaprogramming

### 1. **Concepts for Type Constraints**

Use C++20 concepts to constrain template parameters and provide better error messages.

```cpp
#include <concepts>

template<typename T>
concept ColorLike = requires(T t) {
    { t.r } -> std::convertible_to<uint8_t>;
    { t.g } -> std::convertible_to<uint8_t>;
    { t.b } -> std::convertible_to<uint8_t>;
    { t.a } -> std::convertible_to<uint8_t>;
};

template<typename T>
concept PointLike = requires(T t) {
    { t.x } -> std::convertible_to<int>;
    { t.y } -> std::convertible_to<int>;
};

template<ColorLike Color>
void renderer::set_draw_color(const Color& c) {
    SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
}

template<PointLike Point>
void renderer::draw_point(const Point& p) {
    SDL_RenderPoint(m_renderer, p.x, p.y);
}
```

### 2. **SFINAE for Function Overloading**

Use SFINAE (Substitution Failure Is Not An Error) to provide different overloads based on type properties.

```cpp
#include <type_traits>

class renderer {
public:
    // Overload for containers of points
    template<typename Container>
    std::enable_if_t<
        std::is_same_v<typename Container::value_type, point> &&
        std::is_same_v<decltype(std::begin(std::declval<Container>())), 
                      typename Container::iterator>
    >
    draw_points(const Container& points) {
        for (const auto& p : points) {
            draw_point(p);
        }
    }
    
    // Overload for C-style arrays
    template<size_t N>
    void draw_points(const point (&points)[N]) {
        for (size_t i = 0; i < N; ++i) {
            draw_point(points[i]);
        }
    }
};
```

### 3. **Constexpr for Compile-Time Computation**

Use `constexpr` to move computations to compile time when possible.

```cpp
struct color {
    uint8_t r, g, b, a;
    
    // Compile-time color creation
    static constexpr color rgb(uint8_t red, uint8_t green, uint8_t blue) {
        return {red, green, blue, 255};
    }
    
    static constexpr color rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
        return {red, green, blue, alpha};
    }
    
    // Compile-time color from hex
    static constexpr color from_hex(uint32_t hex) {
        return {
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF),
            255
        };
    }
    
    // Common colors as compile-time constants
    static constexpr color white() { return from_hex(0xFFFFFF); }
    static constexpr color black() { return from_hex(0x000000); }
    static constexpr color red() { return from_hex(0xFF0000); }
    static constexpr color green() { return from_hex(0x00FF00); }
    static constexpr color blue() { return from_hex(0x0000FF); }
};

// Usage - computed at compile time
constexpr auto bg_color = color::from_hex(0x2E3440);
```

---

## Memory Management Patterns

### 1. **Unique Ownership with std::unique_ptr**

For resources that have clear single ownership, use custom deleters with `std::unique_ptr`.

```cpp
struct sdl_surface_deleter {
    void operator()(SDL_Surface* surface) const {
        if (surface) {
            SDL_FreeSurface(surface);
        }
    }
};

using unique_surface = std::unique_ptr<SDL_Surface, sdl_surface_deleter>;

class surface {
    unique_surface m_surface;
    
public:
    explicit surface(unique_surface surf) : m_surface(std::move(surf)) {}
    
    static surface create(int width, int height, uint32_t format) {
        SDL_Surface* surf = SDL_CreateSurface(width, height, format);
        if (!surf) {
            throw error::from_sdl();
        }
        return surface(unique_surface(surf));
    }
    
    SDL_Surface* get() const { return m_surface.get(); }
};
```

### 2. **Shared Ownership for Reference-Counted Resources**

Some SDL resources may benefit from shared ownership semantics.

```cpp
class shared_texture {
    std::shared_ptr<SDL_Texture> m_texture;
    
public:
    explicit shared_texture(SDL_Texture* tex) 
        : m_texture(tex, [](SDL_Texture* t) { 
            if (t) SDL_DestroyTexture(t); 
          }) {}
    
    // Copy semantics work naturally with shared_ptr
    shared_texture(const shared_texture&) = default;
    shared_texture& operator=(const shared_texture&) = default;
    
    SDL_Texture* get() const { return m_texture.get(); }
    long use_count() const { return m_texture.use_count(); }
};
```

### 3. **Custom Allocators for Performance**

For high-performance scenarios, provide custom allocators.

```cpp
template<typename T>
class sdl_allocator {
public:
    using value_type = T;
    
    T* allocate(size_t n) {
        void* ptr = SDL_malloc(n * sizeof(T));
        if (!ptr) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(ptr);
    }
    
    void deallocate(T* ptr, size_t) {
        SDL_free(ptr);
    }
};

// Usage with STL containers
using sdl_vector = std::vector<uint8_t, sdl_allocator<uint8_t>>;
```

---

## Event System Design

### 1. **Type-Safe Event Variants**

Use `std::variant` to create a type-safe event system.

```cpp
#include <variant>

// Individual event types
struct quit_event {
    uint32_t timestamp;
};

struct key_event {
    enum class state { pressed, released };
    uint32_t timestamp;
    state key_state;
    SDL_Scancode scancode;
    SDL_Keycode keycode;
    uint16_t mod;
};

struct mouse_button_event {
    enum class state { pressed, released };
    enum class button { left = 1, middle = 2, right = 3 };
    uint32_t timestamp;
    state button_state;
    button mouse_button;
    int x, y;
    uint8_t clicks;
};

// Event variant
using event = std::variant<
    quit_event,
    key_event,
    mouse_button_event
    // ... other event types
>;

// Event conversion from SDL
event from_sdl_event(const SDL_Event& sdl_ev) {
    switch (sdl_ev.type) {
        case SDL_EVENT_QUIT:
            return quit_event{sdl_ev.quit.timestamp};
            
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            return key_event{
                sdl_ev.key.timestamp,
                sdl_ev.type == SDL_EVENT_KEY_DOWN ? key_event::state::pressed : key_event::state::released,
                sdl_ev.key.scancode,
                sdl_ev.key.key,
                sdl_ev.key.mod
            };
            
        // ... other cases
        
        default:
            throw std::runtime_error("Unsupported event type");
    }
}
```

### 2. **Range-Based Event Polling**

Provide a range-based interface for event polling.

```cpp
class event_range {
    std::vector<event> m_events;
    
public:
    event_range() {
        SDL_Event sdl_event;
        while (SDL_PollEvent(&sdl_event)) {
            m_events.push_back(from_sdl_event(sdl_event));
        }
    }
    
    auto begin() const { return m_events.begin(); }
    auto end() const { return m_events.end(); }
    bool empty() const { return m_events.empty(); }
    size_t size() const { return m_events.size(); }
};

// Usage
for (const auto& ev : laya::poll_events()) {
    std::visit([](const auto& event) {
        using T = std::decay_t<decltype(event)>;
        if constexpr (std::is_same_v<T, quit_event>) {
            // Handle quit
        } else if constexpr (std::is_same_v<T, key_event>) {
            // Handle key
        }
        // ... other event types
    }, ev);
}
```

### 3. **Event Filtering and Handling**

Provide utilities for event filtering and handling.

```cpp
template<typename EventType>
class event_filter {
    std::function<bool(const EventType&)> m_predicate;
    
public:
    template<typename Predicate>
    explicit event_filter(Predicate&& pred) : m_predicate(std::forward<Predicate>(pred)) {}
    
    bool operator()(const event& ev) const {
        if (auto* typed_event = std::get_if<EventType>(&ev)) {
            return m_predicate(*typed_event);
        }
        return false;
    }
};

// Usage
auto key_filter = event_filter<key_event>([](const key_event& ev) {
    return ev.key_state == key_event::state::pressed;
});

for (const auto& ev : laya::poll_events()) {
    if (key_filter(ev)) {
        // Handle pressed keys only
    }
}
```

---

## Zero-Overhead Abstractions

### 1. **Inline Functions for Simple Wrappers**

Mark simple wrapper functions as `inline` to ensure zero overhead.

```cpp
class window {
    SDL_Window* m_window;
    
public:
    inline void show() { SDL_ShowWindow(m_window); }
    inline void hide() { SDL_HideWindow(m_window); }
    inline void maximize() { SDL_MaximizeWindow(m_window); }
    inline void minimize() { SDL_MinimizeWindow(m_window); }
    
    inline size size() const {
        int w, h;
        SDL_GetWindowSize(m_window, &w, &h);
        return {w, h};
    }
};
```

### 2. **Template Specialization for Performance**

Use template specialization to optimize for specific types.

```cpp
// Generic version
template<typename T>
void copy_pixels(T* dest, const T* src, size_t count) {
    std::copy(src, src + count, dest);
}

// Optimized specialization for common pixel formats
template<>
void copy_pixels<uint32_t>(uint32_t* dest, const uint32_t* src, size_t count) {
    SDL_memcpy(dest, src, count * sizeof(uint32_t));
}
```

### 3. **Compile-Time Polymorphism**

Use CRTP (Curiously Recurring Template Pattern) instead of virtual functions where possible.

```cpp
template<typename Derived>
class drawable {
public:
    void draw(renderer& ren) {
        static_cast<Derived*>(this)->draw_impl(ren);
    }
};

class sprite : public drawable<sprite> {
public:
    void draw_impl(renderer& ren) {
        // Sprite-specific drawing
    }
};

class text : public drawable<text> {
public:
    void draw_impl(renderer& ren) {
        // Text-specific drawing
    }
};
```

---

## Testing and Validation

### 1. **Compile-Time Tests**

Use `static_assert` to validate assumptions at compile time.

```cpp
// Ensure our wrappers have the same size as SDL types
static_assert(sizeof(laya::color) == 4, "Color should be 4 bytes");
static_assert(sizeof(laya::point) == 8, "Point should be 8 bytes");
static_assert(sizeof(laya::rect) == 16, "Rect should be 16 bytes");

// Ensure enum values match SDL constants
static_assert(static_cast<uint32_t>(window_flags::resizable) == SDL_WINDOW_RESIZABLE);
static_assert(static_cast<uint32_t>(window_flags::borderless) == SDL_WINDOW_BORDERLESS);
```

### 2. **RAII Validation**

Test that resources are properly managed.

```cpp
#include <doctest/doctest.h>

TEST_CASE("Window RAII") {
    laya::init();
    
    {
        laya::window win("Test", {800, 600});
        // Window should be created successfully
        CHECK(win.native_handle() != nullptr);
    }
    // Window should be automatically destroyed here
    
    laya::quit();
}
```

### 3. **Performance Benchmarks**

Ensure zero-overhead abstractions actually have zero overhead.

```cpp
#include <chrono>

void benchmark_raw_sdl() {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000000; ++i) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    // Record timing
}

void benchmark_laya_wrapper() {
    laya::renderer ren(window);
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000000; ++i) {
        ren.set_draw_color(laya::color::red());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    // Should have identical timing to raw SDL
}
```

---

## Conclusion

These wrapping techniques ensure that laya provides a safe, modern C++ interface to SDL3 without sacrificing performance. By leveraging C++20 features like concepts, constexpr, and RAII, we can create abstractions that are both safer and more expressive than the underlying C API while maintaining zero runtime overhead.
