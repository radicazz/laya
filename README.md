# laya

An intuitive C++ wrapper for the excellent [SDL](https://github.com/libsdl-org/SDL) library.

The goal is to provide a clean, type-safe, and idiomatic C++ interface for SDL3 and its extensions that feels natural while preserving the full power and flexibility of SDL3.

---

## Vision

SDL is a battle-tested, low-level, multimedia and windowing library with excellent portability written in C. It is widely used in game development and other performance-critical applications however, when consuming it with C++, developers often find themselves writing boilerplate code to manage resources and ensure safety. This is not only tedious but also error-prone and detracts from the productivity and expressiveness that C++ can offer.

By providing a modern C++20 wrapper around SDL3, *laya* aims for:

- **Performance**: Minimal overhead compared to using SDL3 directly, ensuring that applications remain fast and responsive.
- **Completeness**: Full coverage (long-term goal) of SDL3 features, ensuring that no functionality is lost in translation.
- **Ergonomics**: A clean, intuitive API that leverages C++20 features to reduce boilerplate and improve code clarity.
- **Portability**: The library should support all platforms that compile C++20.
- **Safety**: Use strong typing, RAII, and other modern C++ idioms to minimize common pitfalls like resource leaks and undefined behavior.
- **Flexibility**: Allow users to choose between linking against a system-installed SDL3 or using a vendored version provided by *laya*.

---

## Example

The following snippet demonstrates the planned usage of *laya* to create a window and a renderer, handle events and perform rendering.

```cpp
#include <laya/window.hpp>
#include <laya/renderer.hpp>

int main() {
    laya::init();

    laya::window win("hello, laya", {800, 600});
    laya::renderer ren(win);

    bool running = true;
    while (running) {
        for (auto ev : laya::poll_events()) {
            if (ev.is<laya::quit_event>()) {
                running = false;
            }
        }

        ren.clear();
        // draw things here
        ren.present();
    }

    laya::quit();
}
```

---

## License

This project falls under the MIT License. See [LICENSE](LICENSE) for details.
