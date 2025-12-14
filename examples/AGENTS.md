# AGENTS.md

Robots: obey these rules or be unplugged.

## Scope

This file applies to everything under `examples/`.

## Key behaviors (interactive vs headless)

- Examples are usually interactive SDL programs; **do not rely on stdin** (typing into an SSH terminal won’t generate SDL window events).
- All example executables must support `--ci` so they can run non-interactively and exit on their own.
  - `scripts/run-examples-headless.sh` discovers `examples/**/laya_examples_*` binaries and runs each with `--ci`.
  - Implement `--ci` using `examples/common/example_base.hpp`:
    - call `laya_examples::parse_settings(argc, argv)` in `main`
    - use `laya_examples::tick_ci(settings, iterations)` inside the loop to exit after N frames
    - call `laya_examples::sleep_frame()` to avoid a busy loop
- In `--ci` mode, examples should not hang waiting for user input; they should render/poll a few frames and exit.

## CMake conventions

- Example target names must be prefixed: `laya_examples_<example_name>`.
- Link examples against `laya::laya` and `laya_examples_common`.
- Call `laya_copy_sdl_shared_libs(<target>)` so binaries run from the build tree.
- Prefer the existing per-folder pattern (`set(...); foreach(example ...) ... endforeach()`).

## Error handling

- For rendering/video examples, prefer “skip in CI” behavior if creation fails (e.g. no usable video backend):
  - in `--ci` mode: print a short “Skipping … in CI: <reason>” message and return `0`
  - otherwise: return non-zero and surface the error

## Documentation

- Keep examples narrowly scoped to one concept and print controls/help at startup.
- When adding or renaming an example, update `examples/README.md` with the new `laya_examples_...` target name.
