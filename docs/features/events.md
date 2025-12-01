# Events

Laya provides a modern, type-safe event handling system using C++20 features like `std::variant` and ranges.

---

## Overview

The event system provides:

- **Type-safe events** - Using `std::variant` for compile-time safety
- **Range-based polling** - Idiomatic C++ iteration over events
- **Zero-copy design** - Efficient event processing
- **Pattern matching** - Using `std::visit` for event handling

---

## Basic Usage

### Event Polling

```cpp
#include <laya/laya.hpp>

int main() {
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

    return 0;
}
```

### Event Types

All events are represented as a `std::variant`:

```cpp
using event = std::variant<
    quit_event,
    window_event,
    key_event,
    // ... other event types
>;
```

---

## Event Types

### Quit Event

Triggered when the application should quit:

```cpp
struct quit_event {
    uint32_t timestamp;
};

// Handling
for (const auto& ev : laya::poll_events()) {
    if (std::holds_alternative<laya::quit_event>(ev)) {
        laya::log_info("Quit requested");
        running = false;
    }
}
```

### Window Event

Window-related events (resize, minimize, etc.):

```cpp
struct window_event {
    uint32_t timestamp;
    window_id window_id;
    // ... event data
};
```

### Key Event

Keyboard events:

```cpp
struct key_event {
    enum class state { pressed, released };
    uint32_t timestamp;
    state key_state;
    // ... key details
};
```

---

## Event Handling Patterns

### Using std::holds_alternative

Check for specific event types:

```cpp
for (const auto& event : laya::poll_events()) {
    if (std::holds_alternative<laya::quit_event>(event)) {
        running = false;
    }
    else if (std::holds_alternative<laya::window_event>(event)) {
        // Handle window events
    }
}
```

### Using std::visit

Pattern matching with visitors:

```cpp
for (const auto& event : laya::poll_events()) {
    std::visit([&](const auto& ev) {
        using T = std::decay_t<decltype(ev)>;

        if constexpr (std::is_same_v<T, laya::quit_event>) {
            running = false;
        }
        else if constexpr (std::is_same_v<T, laya::window_event>) {
            laya::log_info("Window event at {}", ev.timestamp);
        }
        else if constexpr (std::is_same_v<T, laya::key_event>) {
            laya::log_info("Key event");
        }
    }, event);
}
```

### Using std::get_if

Access event data safely:

```cpp
for (const auto& event : laya::poll_events()) {
    if (auto* quit = std::get_if<laya::quit_event>(&event)) {
        laya::log_info("Quit at timestamp {}", quit->timestamp);
        running = false;
    }
}
```

---

## Practical Examples

### Simple Event Loop

```cpp
#include <laya/laya.hpp>

int main() {
    laya::context ctx{laya::subsystem::video};
    laya::window win{"Event Demo", {800, 600}};
    laya::renderer ren{win};

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
        using T = std::decay_t<decltype(ev)>;
        if constexpr (std::is_same_v<T, laya::quit_event>) {
            running = false;
        }
    }, event);
};

for (const auto& event : laya::poll_events()) {
    process_events(event);
}
```

### Custom Event Handler

```cpp
class event_handler {
public:
    void operator()(const laya::quit_event& ev) {
        laya::log_info("Quit event");
        should_quit = true;
    }

    void operator()(const laya::window_event& ev) {
        laya::log_info("Window event");
    }

    void operator()(const auto& ev) {
        // Handle all other events
    }

    bool should_quit = false;
};

// Usage
event_handler handler;
for (const auto& event : laya::poll_events()) {
    std::visit(handler, event);
    if (handler.should_quit) break;
}
```

---

## Performance Considerations

### Zero-Copy Design

Events are processed without copying:

```cpp
// Events are stored internally and accessed via references
for (const auto& event : laya::poll_events()) {
    // 'event' is a const reference - no copying
}
```

### Efficient Polling

The event range only polls once per iteration:

```cpp
// Single poll for all events
for (const auto& event : laya::poll_events()) {
    // Process events
}

// Don't do this - polls on every iteration
while (!laya::poll_events().empty()) {  // ❌ Creates new range each time
    // ...
}
```

---

## See Also

- **[Getting Started](../getting-started.md)** - Basic setup and usage
- **[Windows](windows.md)** - Window event handling
- **[Architecture](../architecture.md)** - Event system design
- **[Examples](../../examples/events/)** - Complete example code
