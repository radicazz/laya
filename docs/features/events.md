# Events

Handle user input with type-safe event polling.

## Basic Event Loop

```cpp
laya::context ctx{laya::subsystem::video};
laya::window win{"Events", {800, 600}};

bool running = true;
while (running) {
    for (const auto& event : laya::poll_events()) {
        if (std::holds_alternative<laya::quit_event>(event)) {
            running = false;
        }
    }
}
```

## Event Types

Quit event:
```cpp
if (std::holds_alternative<laya::quit_event>(event)) {
    running = false;
}
```

Key events:
```cpp
if (std::holds_alternative<laya::key_event>(event)) {
    auto& key = std::get<laya::key_event>(event);
    // Handle keyboard input
}
```

Mouse events:
```cpp
if (std::holds_alternative<laya::mouse_button_event>(event)) {
    auto& mouse = std::get<laya::mouse_button_event>(event);
    // Handle mouse clicks
}
```

## Pattern Matching

Using `std::visit`:
```cpp
for (const auto& event : laya::poll_events()) {
    std::visit([&](const auto& ev) {
        using T = std::decay_t<decltype(ev)>;

        if constexpr (std::is_same_v<T, laya::quit_event>) {
            running = false;
        }
        else if constexpr (std::is_same_v<T, laya::key_event>) {
            // Handle key
        }
    }, event);
}
```

## Example

Complete event handling:
```cpp
laya::context ctx{laya::subsystem::video};
laya::window win{"Input", {800, 600}};
laya::renderer ren{win};

bool running = true;
while (running) {
    for (const auto& event : laya::poll_events()) {
        if (std::holds_alternative<laya::quit_event>(event)) {
            running = false;
        }
    }

    ren.clear(laya::color::black());
    ren.present();
}
```

    bool running = true;
    while (running) {
        // Process all pending events
        for (const auto& event : laya::poll_events()) {
            if (std::holds_alternative<laya::quit_event>(event)) {
                running = false;
            }
        }

        // Render
        ren.clear(laya::color::black());
        ren.present();
    }

    return 0;
}
```

### Event Logging

```cpp
for (const auto& event : laya::poll_events()) {
    std::visit([](const auto& ev) {
        using T = std::decay_t<decltype(ev)>;

        if constexpr (std::is_same_v<T, laya::quit_event>) {
            laya::log_info("Quit event");
        }
        else if constexpr (std::is_same_v<T, laya::window_event>) {
            laya::log_info("Window event");
        }
        else if constexpr (std::is_same_v<T, laya::key_event>) {
            laya::log_info("Key event");
        }
    }, event);
}
```

### Filtering Events

```cpp
// Count specific event types
int quit_count = 0;
int window_count = 0;

for (const auto& event : laya::poll_events()) {
    if (std::holds_alternative<laya::quit_event>(event)) {
        quit_count++;
    }
    else if (std::holds_alternative<laya::window_event>(event)) {
        window_count++;
    }
}

laya::log_info("Quit events: {}, Window events: {}",
               quit_count, window_count);
```

---

## Advanced Usage

### Event Processing with Lambda

```cpp
auto process_events = [&running](const laya::event& event) {
    std::visit([&running](const auto& ev) {
        ```
