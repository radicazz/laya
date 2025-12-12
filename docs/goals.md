# Why Laya Exists

Laya was built to keep SDL3's speed and reach while removing the footguns that come with its C API.

## What we optimize for

- **Safety first** – RAII everywhere, no dangling handles, no forgotten destroys.
- **Modern C++20 ergonomics** – strong types, `std::variant` events, `std::format` logging.
- **Thin, predictable layer** – mirrors SDL behavior with near‑zero overhead.
- **Cross‑platform sanity** – the same patterns work on Windows, macOS, and Linux.

## What we won't do

- Become a game engine or UI toolkit.
- Hide SDL3 behind heavy abstractions.
- Add features that change how SDL behaves.

## The pitch in one line

Use SDL3 with confidence and modern C++ comfort, without trading away performance or control.
