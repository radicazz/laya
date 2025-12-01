# Rendering

Laya provides a modern C++ interface for 2D rendering, wrapping SDL3's renderer functionality with RAII and type safety.

---

## Overview

The rendering system provides:

- **RAII resource management** - Automatic cleanup on destruction
- **Type-safe colors** - Compile-time color definitions
- **Strong geometric types** - Prevent parameter mix-ups
- **Move semantics** - Transfer ownership safely
- **Exception-based errors** - Automatic error propagation

---

## Basic Usage

### Creating a Renderer

```cpp
#include <laya/laya.hpp>

int main() {
    laya::context ctx{laya::subsystem::video};
    
    laya::window win{"Rendering Example", {800, 600}};
    laya::renderer renderer{win};
    
    // Renderer is automatically destroyed when going out of scope
    return 0;
}
```

### Simple Drawing

```cpp
laya::window win{"Draw", {800, 600}};
laya::renderer ren{win};

bool running = true;
while (running) {
    for (const auto& event : laya::poll_events()) {
        if (std::holds_alternative<laya::quit_event>(event)) {
            running = false;
        }
    }
    
    // Clear with black
    ren.clear(laya::color::black());
    
    // Draw red rectangle
    ren.set_draw_color(laya::color::red());
    ren.fill_rect({100, 100, 200, 150});
    
    // Present to screen
    ren.present();
}
```

---

## Colors

### Color Structure

```cpp
struct color {
    uint8_t r, g, b, a;
};
```

### Creating Colors

```cpp
// Direct construction
laya::color c1{255, 128, 0, 255};  // Orange, fully opaque

// RGB construction (alpha = 255)
auto c2 = laya::color::rgb(255, 0, 0);  // Red

// RGBA construction
auto c3 = laya::color::rgba(0, 255, 0, 128);  // Semi-transparent green

// From hex value (compile-time)
constexpr auto c4 = laya::color::from_hex(0xFF8000);  // Orange
```

### Predefined Colors

```cpp
laya::color::white()
laya::color::black()
laya::color::red()
laya::color::green()
laya::color::blue()
laya::color::yellow()
laya::color::cyan()
laya::color::magenta()
```

---

## Drawing Operations

### Clear Screen

```cpp
// Clear with current draw color
ren.clear();

// Clear with specific color
ren.clear(laya::color{100, 149, 237});  // Cornflower blue
```

### Draw Color

```cpp
// Set draw color for subsequent operations
ren.set_draw_color(laya::color::red());

// Get current draw color
auto color = ren.draw_color();
```

### Rectangles

```cpp
// Outline rectangle
ren.set_draw_color(laya::color::red());
ren.draw_rect({100, 100, 200, 150});

// Filled rectangle
ren.set_draw_color(laya::color::blue());
ren.fill_rect({350, 100, 200, 150});
```

### Present

```cpp
// Show rendered content on screen
ren.present();
```

---

## Geometric Types

### Rect

```cpp
struct rect {
    float x, y;      // Position
    float w, h;      // Size
};

// Usage
laya::rect r{100.0f, 100.0f, 200.0f, 150.0f};
ren.fill_rect(r);
```

---

## Renderer Flags

```cpp
enum class renderer_flags : uint32_t {
    software,           // Software rendering
    accelerated,        // Hardware acceleration
    present_vsync       // Vertical sync
};
```

---

## Error Handling

Renderer creation failures throw exceptions:

```cpp
try {
    laya::window win{"Test", {800, 600}};
    laya::renderer ren{win};
    // Renderer created successfully
} catch (const laya::error& e) {
    laya::log_critical("Renderer creation failed: {}", e.what());
    return 1;
}
```

---

## Move Semantics

Renderers are non-copyable but movable:

```cpp
laya::renderer create_renderer(laya::window& win) {
    laya::renderer ren{win};
    return ren;  // Move, not copy
}

int main() {
    laya::context ctx{laya::subsystem::video};
    laya::window win{"Test", {800, 600}};
    
    laya::renderer ren1{win};
    laya::renderer ren2 = std::move(ren1);  // Transfer ownership
    // ren1 is now invalid, ren2 owns the renderer
    
    return 0;
}
```

---

## Native Handle

Access the underlying SDL renderer for interop:

```cpp
laya::renderer ren{win};
SDL_Renderer* native = ren.native_handle();

// Use with SDL3 functions if needed
// (though you should rarely need this)
```

---

## Practical Examples

### Simple Graphics Application

```cpp
#include <laya/laya.hpp>

int main() {
    try {
        laya::context ctx{laya::subsystem::video};
        
        laya::window win{"Graphics Demo", {800, 600}};
        laya::renderer ren{win};
        
        bool running = true;
        while (running) {
            // Handle events
            for (const auto& event : laya::poll_events()) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    running = false;
                }
            }
            
            // Clear with dark blue
            ren.clear(laya::color{25, 25, 112});
            
            // Draw some rectangles
            ren.set_draw_color(laya::color::red());
            ren.fill_rect({50, 50, 100, 100});
            
            ren.set_draw_color(laya::color::green());
            ren.fill_rect({200, 50, 100, 100});
            
            ren.set_draw_color(laya::color::blue());
            ren.fill_rect({350, 50, 100, 100});
            
            // Present to screen
            ren.present();
        }
        
    } catch (const laya::error& e) {
        laya::log_critical("Error: {}", e.what());
        return 1;
    }
    
    return 0;
}
```

### Animated Rectangle

```cpp
#include <laya/laya.hpp>
#include <cmath>

int main() {
    laya::context ctx{laya::subsystem::video};
    laya::window win{"Animation", {800, 600}};
    laya::renderer ren{win};
    
    bool running = true;
    float time = 0.0f;
    
    while (running) {
        for (const auto& event : laya::poll_events()) {
            if (std::holds_alternative<laya::quit_event>(event)) {
                running = false;
            }
        }
        
        // Clear screen
        ren.clear(laya::color::black());
        
        // Animated rectangle
        float x = 400.0f + std::sin(time) * 200.0f;
        float y = 300.0f + std::cos(time) * 200.0f;
        
        ren.set_draw_color(laya::color::red());
        ren.fill_rect({x - 25.0f, y - 25.0f, 50.0f, 50.0f});
        
        // Present
        ren.present();
        
        time += 0.016f;  // ~60 FPS
    }
    
    return 0;
}
```

---

## See Also

- **[Getting Started](../getting-started.md)** - Basic setup and usage
- **[Windows](windows.md)** - Window management
- **[Architecture](../architecture.md)** - Design patterns
- **[Examples](../../examples/rendering/)** - Complete example code
