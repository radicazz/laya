# Windows

Create and manage windows with automatic cleanup and type safety.

## Creating a Window

```cpp
#include <laya/laya.hpp>

int main() {
    laya::context ctx{laya::subsystem::video};
    laya::window win{"My Window", {800, 600}};

    // Window cleans itself up when it goes out of scope
}
```

With flags and an explicit position:

```cpp
laya::window win{
    "Resizable Window",
    {1024, 768},
    std::optional<laya::position>{laya::position{100, 100}},
    laya::window_flags::resizable | laya::window_flags::always_on_top
};
```

## Window Properties

Get and set window properties:

```cpp
// Size and position
auto sz = win.get_size();
win.set_size({1024, 768});

auto pos = win.get_position();
win.set_position({100, 100});

// Title
win.set_title("New Title");

// State
win.show();
win.hide();
win.maximize();
win.minimize();
win.restore();
win.set_fullscreen(true);
win.set_borderless(true);
win.set_resizable(true);
win.set_minimum_size({640, 480});
win.set_maximum_size({1920, 1080});
win.set_mouse_grab(true);
win.set_keyboard_grab(true);
win.set_opacity(0.9f);

// Queries
auto min_size = win.get_minimum_size();
auto max_size = win.get_maximum_size();
float opacity = win.get_opacity();
```

## Window Events

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

Handle window events:

```cpp
for (const auto& event : laya::poll_events()) {
    if (std::holds_alternative<laya::quit_event>(event)) {
        running = false;
    }
}
```

## Examples

Basic window:
```cpp
laya::context ctx{laya::subsystem::video};
laya::window win{"My App", {800, 600}, laya::window_flags::resizable};
```

Fullscreen:
```cpp
laya::window win{"Game", {1920, 1080}, laya::window_flags::fullscreen};
```

Borderless:
```cpp
laya::window win{
    "Borderless",
    {800, 600},
    laya::window_flags::borderless | laya::window_flags::resizable
};
```
