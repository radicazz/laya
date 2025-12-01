# Windows

Laya provides a modern C++ interface for window management, wrapping SDL3's window functionality with RAII and type safety.

---

## Overview

The window system provides:

- **RAII resource management** - Automatic cleanup on destruction
- **Type-safe flags** - Compile-time checked window properties
- **Strong types** - Prevent parameter mix-ups
- **Move semantics** - Transfer ownership safely
- **Exception-based errors** - Automatic error propagation

---

## Basic Usage

### Creating a Window

```cpp
#include <laya/laya.hpp>

int main() {
    laya::context ctx{laya::subsystem::video};
    
    // Simple window
    laya::window win{"My Window", {800, 600}};
    
    // Window with flags
    laya::window win2{
        "Resizable Window", 
        {1024, 768}, 
        laya::window_flags::resizable
    };
    
    // Window is automatically destroyed when going out of scope
    return 0;
}
```

### Window Flags

Type-safe enum for window creation flags:

```cpp
enum class window_flags : uint32_t {
    none = 0,
    fullscreen,
    opengl,
    vulkan,
    metal,
    hidden,
    borderless,
    resizable,
    minimized,
    maximized,
    mouse_grabbed,
    input_focus,
    mouse_focus,
    external,
    modal,
    high_pixel_density,
    mouse_capture,
    mouse_relative_mode,
    always_on_top,
    utility,
    tooltip,
    popup_menu,
    keyboard_grabbed,
    input_grabbed,
    transparent,
    not_focusable
};
```

### Combining Flags

Use bitwise OR to combine flags:

```cpp
auto flags = laya::window_flags::resizable | 
             laya::window_flags::borderless;

laya::window win{"Combined Flags", {800, 600}, flags};
```

---

## Window Properties

### Size and Position

Strong types prevent parameter mix-ups:

```cpp
// Get window size
auto sz = win.size();
laya::log_info("Window size: {}x{}", sz.width, sz.height);

// Set window size
win.set_size({1024, 768});

// Get window position
auto pos = win.position();
laya::log_info("Window position: ({}, {})", pos.x, pos.y);

// Set window position
win.set_position({100, 100});
```

### Title

```cpp
// Set window title
win.set_title("New Title");

// Get window title
std::string_view title = win.title();
```

### Window State

```cpp
// Show/hide
win.show();
win.hide();

// Maximize/minimize/restore
win.maximize();
win.minimize();
win.restore();

// Raise window
win.raise();

// Fullscreen
win.set_fullscreen(true);
```

---

## Window ID

Each window has a unique identifier:

```cpp
laya::window win{"Test", {800, 600}};

auto id = win.id();
if (id) {
    laya::log_info("Window ID: {}", id.value());
}
```

---

## Move Semantics

Windows are non-copyable but movable:

```cpp
laya::window create_window() {
    laya::window win{"Temporary", {800, 600}};
    return win;  // Move, not copy
}

int main() {
    laya::context ctx{laya::subsystem::video};
    
    laya::window win1{"First", {800, 600}};
    laya::window win2 = std::move(win1);  // Transfer ownership
    // win1 is now invalid, win2 owns the window
    
    laya::window win3 = create_window();  // Move from returned value
    
    return 0;
}
```

---

## Error Handling

Window creation failures throw exceptions:

```cpp
try {
    laya::window win{"Test", {800, 600}};
    // Window created successfully
} catch (const laya::error& e) {
    laya::log_critical("Window creation failed: {}", e.what());
    return 1;
}
```

---

## Native Handle

Access the underlying SDL window for interop:

```cpp
laya::window win{"Test", {800, 600}};
SDL_Window* native = win.native_handle();

// Use with SDL3 functions if needed
// (though you should rarely need this)
```

---

## Practical Examples

### Simple Application Window

```cpp
#include <laya/laya.hpp>

int main() {
    try {
        laya::context ctx{laya::subsystem::video};
        
        laya::window win{
            "My Application", 
            {800, 600}, 
            laya::window_flags::resizable
        };
        
        bool running = true;
        while (running) {
            for (const auto& event : laya::poll_events()) {
                if (std::holds_alternative<laya::quit_event>(event)) {
                    running = false;
                }
            }
        }
        
    } catch (const laya::error& e) {
        laya::log_critical("Error: {}", e.what());
        return 1;
    }
    
    return 0;
}
```

### Fullscreen Window

```cpp
laya::window win{
    "Fullscreen Game", 
    {1920, 1080}, 
    laya::window_flags::fullscreen
};
```

### Borderless Window

```cpp
laya::window win{
    "Borderless", 
    {800, 600}, 
    laya::window_flags::borderless | laya::window_flags::resizable
};
```

---

## See Also

- **[Getting Started](../getting-started.md)** - Basic setup and usage
- **[Rendering](rendering.md)** - Creating renderers for windows
- **[Events](events.md)** - Handling window events
- **[Examples](../../examples/rendering/)** - Complete example code
