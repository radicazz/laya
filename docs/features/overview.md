# Key Features

- **RAII for SDL** – windows, renderers, textures, and contexts clean themselves up.
- **Strong types** – sizes, positions, colors, and flags are type-safe so common mixups fail to compile.
- **Event ranges** – pull events with a range-based loop and handle them via `std::variant`.
- **Rendering helpers** – straightforward renderer API with handy color presets.
- **Logging built-in** – `std::format`-checked messages routed through SDL's logger.
- **SDL handled for you** – fetches SDL3 automatically or uses your existing targets when provided.
- **Cross-platform** – works wherever SDL3 and C++20 do.

Want details? Check the examples in the repo or peek at the headers—no sprawling API tables here.
