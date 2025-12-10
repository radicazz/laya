# Rendering

Draw 2D graphics with automatic resource cleanup and type safety.

## Basic Drawing

```cpp
laya::context ctx{laya::subsystem::video};
laya::window win{"Draw", {800, 600}};
laya::renderer ren{win};

bool running = true;
while (running) {
    for (const auto& event : laya::poll_events()) {
        if (std::holds_alternative<laya::quit_event>(event)) {
            running = false;
        }
    }

    ren.clear(laya::color::black());
    ren.set_draw_color(laya::color::red());
    ren.fill_rect({100, 100, 200, 150});
    ren.present();
}
```

## Colors

Create colors:
```cpp
laya::color c1{255, 128, 0, 255};              // RGBA
auto c2 = laya::color::rgb(255, 0, 0);         // RGB (alpha = 255)
constexpr auto c3 = laya::color::from_hex(0xFF8000);  // From hex
```

Predefined colors:
```cpp
laya::color::white()
laya::color::black()
laya::color::red()
laya::color::green()
laya::color::blue()
```

## Drawing Shapes
laya::color::magenta()
```

---

Rectangles:
```cpp
ren.set_draw_color(laya::color::red());
ren.draw_rect({100, 100, 200, 150});    // Outline
ren.fill_rect({350, 100, 200, 150});    // Filled
```

Clear and present:
```cpp
ren.clear(laya::color::black());  // Clear screen
ren.present();                     // Show what you drew
```

## Example

Simple animation:
```cpp
laya::context ctx{laya::subsystem::video};
laya::window win{"Animation", {800, 600}};
laya::renderer ren{win};

float x = 0;
bool running = true;

while (running) {
    for (const auto& event : laya::poll_events()) {
        if (std::holds_alternative<laya::quit_event>(event)) {
            running = false;
        }
    }

    ren.clear(laya::color::black());
    ren.set_draw_color(laya::color::red());
    ren.fill_rect({x, 250, 50, 50});
    ren.present();

    x += 1;
    if (x > 800) x = 0;
}
```
    laya::context ctx{laya::subsystem::video};
    laya::window win{"Test", {800, 600}};

    laya::renderer ren1{win};
    laya::renderer ren2 = std::move(ren1);  // Transfer ownership
    // ren1 is now invalid, ren2 owns the renderer

    return 0;
}
```

---

## Textures and Surfaces

You can upload pixels from an SDL surface, tint them, and draw them like any other primitive. This is useful when loading BMP files (built-in) or PNG files (via SDL_image).

```cpp
laya::context ctx{laya::subsystem::video};
laya::window win{"Textures", {800, 600}};
laya::renderer ren{win};

// Load a surface from disk.
auto surf = laya::surface::load_bmp("assets/logo.bmp");
// Create a texture bound to the renderer.
auto tex = laya::texture::from_surface(ren, surf);
tex.set_color_mod(laya::color{255, 255, 255});

ren.clear(laya::color::black());
ren.render(tex, {100, 100, 256, 256});
ren.present();
```

### Limitations

- `surface::load_png` and `surface::save_png` currently throw until SDL_image support is integrated.
- `texture::load_png` mirrors this limitation because it depends on surfaces.
- Regional texture locking is supported, but surfaces generally do not require locking in SDL3; `surface::must_lock()` returns `false` for now.

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

}
