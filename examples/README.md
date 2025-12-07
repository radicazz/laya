# Laya Examples

This directory contains example programs demonstrating various features of the laya library.

## Running Examples

All executables accept `--ci` to run a fixed number of iterations and exit automatically.

```bash
# Linux/macOS
./examples/events/laya_examples_event_view_basic --ci

# Windows
.\examples\events\Release\laya_examples_event_view_basic.exe --ci
```

Shared helpers for argument parsing and CI timing live in `examples/common/example_base.hpp`.

## Available Examples

### events/

- `laya_examples_event_view_basic`: zero-allocation `events_view`, quit on ESC/close.
- `laya_examples_event_range_multipass`: `events_range` with multi-pass processing and typed window data helpers.
- `laya_examples_window_event_data`: focused window event data access (`get_position`, `get_size`, `get_display`).
- `laya_examples_quit_and_escape`: minimal quit handling via close button or ESC.

### logging/

- `laya_examples_logging_basics_levels`: log levels and format strings.
- `laya_examples_logging_categories_priority`: category logging and temporary priority overrides.
- `laya_examples_logging_color_output`: optional colored output (skips in CI).
- `laya_examples_logging_render_loop`: render loop with periodic logging and quit handling.

### rendering/

- `laya_examples_render_clear_screen`: clear/present loop with quit handling.
- `laya_examples_render_draw_primitives`: rectangles, lines, points, and color guards.
- `laya_examples_render_quit_with_events`: minimal renderer reacting to quit/ESC.

## Adding New Examples

- Support `--ci` (see `example_base.hpp` helpers).
- Keep examples narrowly scoped to a single concept.
- Add the target and a short bullet to this README.
