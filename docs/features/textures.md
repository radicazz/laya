# Textures

GPU resources backed by SDL_Renderer for fast blitting, tinting, and scaling.

## Creating Textures

```cpp
#include <laya/laya.hpp>

laya::context ctx{laya::subsystem::video};
laya::window window{"Textures", {800, 600}};
laya::renderer renderer{window};

laya::texture tex_from_args{renderer, laya::texture_args{
    .format = laya::pixel_format::rgba32,
    .size = {256, 256},
    .access = laya::texture_access::streaming,
}};

auto from_surface = laya::texture::from_surface(renderer, laya::surface::load_bmp("sprite.bmp"));
```

> **PNG support** — `texture::load_png` depends on `surface::load_png`, so it currently throws until SDL_image wiring lands.

## Updating Pixels

Upload CPU buffers directly:

```cpp
std::vector<std::uint32_t> pixels(256 * 256, 0xFF00FF00);
tex_from_args.update(pixels.data(), 256 * sizeof(std::uint32_t));

laya::rect region{32, 32, 64, 64};
tex_from_args.update(region, pixels.data(), 256 * sizeof(std::uint32_t));
```

Or lock for streaming writes:

```cpp
{
    auto lock = tex_from_args.lock();
    auto* row = static_cast<std::uint8_t*>(lock.pixels());
    for (int y = 0; y < tex_from_args.size().height; ++y) {
        std::fill_n(row, tex_from_args.size().width * 4, 0x7F);
        row += lock.pitch();
    }
}
```

Regional locking leverages SDL3’s built-in support via `texture::lock(const rect&)`.

## Rendering

Renderer helpers cover common blit/transform combos:

```cpp
laya::renderer ren{window};
ren.clear();
ren.render(from_surface, {100, 100});                             // position
ren.render(from_surface, {200, 200, 64, 64});                      // destination rect
ren.render(from_surface, {0, 0, 32, 32}, {320, 200, 64, 64});      // src/dst
ren.render(from_surface, {320, 320, 64, 64}, 45.0);                // rotation
ren.render(from_surface, {320, 320, 64, 64}, 0.0, {32, 32});       // custom pivot
ren.render(from_surface, {0, 0, 128, 128}, {400, 100, 128, 128},
           0.0, {0, 0}, laya::flip_mode::horizontal);
ren.present();
```

All rectangle coordinates convert to `SDL_FRect` internally, so integer inputs stay precise while enabling subpixel rendering.

## Modulation & State

```cpp
from_surface.set_alpha_mod(200);
from_surface.set_color_mod({255, 200, 200});
from_surface.set_blend_mode(laya::blend_mode::blend);
from_surface.set_scale_mode(laya::scale_mode::linear);

auto alpha = from_surface.get_alpha_mod();
auto color = from_surface.get_color_mod();
```

Scale modes map directly to SDL scale filters; `nearest` and `linear` are available today. `SDL_SCALEMODE_BEST` is unavailable in SDL3 and purposely omitted.

## Metadata

Texture instances cache their size/format/access metadata when created, so queries avoid expensive `SDL_GetTextureProperties` calls at runtime:

```cpp
auto size = from_surface.size();           // returns cached dimensions
auto format = from_surface.format();
auto access = from_surface.access();
```

## Limitations & Future Work

- PNG helpers throw until SDL_image integration is complete.
- `texture::from_surface` currently duplicates metadata queries; future updates will streamline this when SDL adds richer creation APIs.
- Renderer helpers currently accept `laya::rect`/`laya::point`; span-based batching is planned for future revisions.
- `texture::load_*` helpers are synchronous; async/background loading is left to higher-level systems.
