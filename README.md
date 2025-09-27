# 🧱 laya

An intuitive wrapper that seamlessly integrates [SDL](https://github.com/libsdl-org/SDL) with modern C++.

The idea behind *laya* is to provide a clean, type-safe and idiomatic C++ interface for SDL and its extensions that feels natural while preserving the full power and flexibility of the underlying C library.

---

## Vision

By carefully wrapping SDL in a modern C++20 interface, *laya* aims to provide:

- **Performance**: Minimal overhead compared to using SDL3 directly, ensuring that applications remain fast and responsive.
- **Completeness**: Full coverage (long-term goal) of SDL3 features, ensuring that no functionality is lost in translation.
- **Ergonomics**: A clean, intuitive API that leverages C++20 features to reduce boilerplate and improve code clarity.
- **Portability**: The library should support all platforms that compile C++20.
- **Safety**: Use strong typing, RAII, and other modern C++ idioms to minimize common pitfalls like resource leaks and undefined behavior.
- **Flexibility**: Allow users to choose between linking against a system-installed SDL or using a vendored version provided by *laya*.

---

## Example

The following snippet demonstrates the planned usage of *laya* to create a window and a renderer, handle events and perform rendering.

```cpp
int laya_app() {
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

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines and code style.

---

## License

This project falls under the MIT License. See [LICENSE.txt](LICENSE.txt) for details.

---
