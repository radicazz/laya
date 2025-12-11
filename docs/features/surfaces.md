# Surfaces

CPU-resident pixel buffers for loading images, running software draw routines, and staging texture uploads.

## Creating Surfaces

```cpp
#include <laya/laya.hpp>

laya::surface from_args{laya::surface_args{
    .size = {256, 256},
    .format = laya::pixel_format::rgba32,
    .flags = laya::surface_flags::rle_optimized,
}};

laya::surface from_size{{128, 128}};                  // Defaults to RGBA32
laya::surface from_bmp = laya::surface::load_bmp("ui/logo.bmp");
```

> **PNG support** — `surface::load_png`/`save_png` currently throw until SDL_image is wired up. Use BMP helpers or provide your own loader if you need PNG today.

## Filling and Blitting

```cpp
from_args.fill(laya::color::black());
from_args.fill_rect({32, 32, 96, 96}, laya::color{255, 0, 0});

std::array<laya::rect, 2> bars{
    laya::rect{0, 0, 64, 256},
    laya::rect{192, 0, 64, 256},
};
from_args.fill_rects(bars, laya::color::white());

from_args.blit(from_bmp, {0, 0, 64, 64}, {160, 160, 64, 64});
from_args.blit(from_bmp, {50, 50});
```

## Transformations

Transformations return new `laya::surface` instances, preserving RAII semantics:

```cpp
auto copy = from_bmp.duplicate();
auto converted = from_bmp.convert(laya::pixel_format::bgra32);
auto scaled = from_bmp.scale({512, 512});
auto flipped = from_bmp.flip(laya::flip_mode::horizontal);
```

Scaling currently uses linear filtering; configurable scale modes will arrive with future renderer updates.

## State Management

```cpp
copy.set_alpha_mod(192);
copy.set_color_mod({200, 255, 200});
copy.set_blend_mode(laya::blend_mode::blend);

if (copy.has_color_key()) {
    copy.clear_color_key();
}
```

Color key values respect the surface pixel format internally by querying SDL’s format metadata.

## Locking Pixels

Use `surface_lock_guard` for direct pixel access. The guard captures the raw pointer/pitch up front and automatically unlocks when destroyed.

```cpp
{
    auto lock = copy.lock();
    std::uint8_t* pixels = static_cast<std::uint8_t*>(lock.pixels());
    const int pitch = lock.pitch();
    // mutate pixels here
} // unlocked automatically
```

SDL3 rarely requires locking for software surfaces; `surface::must_lock()` returns `false` until SDL exposes richer metadata, but the guard keeps the API consistent.

## File IO

```cpp
from_args.save_bmp("out/debug.bmp");
// from_args.save_png("out/debug.png"); // throws until SDL_image support
```

## Integrating with Textures

Surfaces are ideal staging buffers for GPU textures:

```cpp
laya::renderer renderer{window};
auto sprite_surface = laya::surface::load_bmp("assets/sprite.bmp");
auto sprite_texture = laya::texture::from_surface(renderer, sprite_surface);
```

See [Textures](textures.md) for the GPU side of the pipeline.

## Limitations & Future Work

- PNG helpers require SDL_image and will throw until that dependency is integrated.
- Scale mode is fixed to linear filtering; configurable scale modes will be added later.
- `surface_args::flags` currently support `rle_optimized`; additional SDL surface flags can be surfaced if needed.
