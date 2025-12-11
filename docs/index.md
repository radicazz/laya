# laya docs

A modern C++20 wrapper around SDL3 that makes multimedia programming safer and more pleasant.

## What is *laya*?

Think of it as SDL3 with the safety rails up. *laya* wraps the C API in idiomatic C++20:

- **Memory Safety** - Resources clean themselves up automatically
- **Type Safety** - Catch bugs at compile time instead of runtime
- **Modern C++** - Concepts, ranges, `std::format` and friends
- **Cross-Platform** - Works everywhere SDL3 and C++20 work

## Quick Example

```cpp
#include <laya/laya.hpp>

int main() {
    laya::context ctx{laya::subsystem::video};

    laya::window win{"My Game", {800, 600}, laya::window_flags::resizable};
    laya::renderer renderer{win};

    bool running = true;
    while (running) {
        for (const auto& event : laya::poll_events()) {
            if (std::holds_alternative<laya::quit_event>(event)) {
                running = false;
            }
        }

        renderer.clear(laya::color::black());
        // Drawing code here
        renderer.present();
    }

    return 0;
}
```

---

## Why Use *laya*?

Resources clean themselves up:
```cpp
{
    laya::window win{"Title", {800, 600}};
    laya::renderer ren{win};
} // Everything freed automatically
```

Strong types catch mistakes:
```cpp
win.set_size({800, 600});      // OK
win.set_position({100, 50});   // OK
// win.set_size({100, 50});    // Won't compile!
```

---

## Next Steps

- **[Getting Started](getting-started.md)** - Installation and first app
- **[Goals](goals.md)** - Design philosophy and objectives
- **[Architecture](architecture.md)** - How it works under the hood
- **[Features](features/windows.md)** - Explore specific capabilities
