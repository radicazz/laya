# Laya Examples

This directory contains example programs demonstrating various features of the laya library.

## Running Examples

### Interactive Mode (Default)

By default, examples run in interactive mode where they wait for user input:

```bash
# Linux/macOS
./examples/events/laya_examples_events

# Windows
.\examples\events\Release\laya_examples_events.exe
```

### CI Mode (Non-Interactive)

For automated testing and CI environments, use the `--ci` flag to run a fixed number of iterations and exit automatically:

```bash
# Linux/macOS
./examples/events/laya_examples_events --ci

# Windows
.\examples\events\Release\laya_examples_events.exe --ci
```

CI mode:

- Runs a predetermined number of iterations (typically 5)
- Exits automatically without waiting for user input
- Prints "Running in CI mode (non-interactive)" at startup
- Reports iteration count on completion

## Available Examples

### events/

Demonstrates event polling mechanisms and type-safe event handling.

**Features shown:**

- `event_view` - Zero-allocation lazy event polling
- `event_range` - Multi-pass event iteration with stored events
- Type-safe window events with `std::variant`
- Helper functions for accessing window event data (`get_position`, `get_size`, `get_display`)
- Pattern matching with `std::visit`

**Window Event Types:**

- Position events (`moved`) - Access with `get_position(event)`
- Size events (`resized`, `size_changed`) - Access with `get_size(event)`
- Display events (`display_changed`) - Access with `get_display(event)`
- State events (`shown`, `hidden`, `minimized`, `maximized`, etc.) - No additional data

**Type-Safe Event Data:**

The window event system uses `std::variant` to provide compile-time type safety:

```cpp
// Old (unsafe):
if (win_event->event_type == window_event::type::resized) {
    int width = win_event->data1;   // Magic field
    int height = win_event->data2;  // What do these mean?
}

// New (type-safe):
if (auto size = laya::get_size(*win_event)) {
    std::cout << "Resized to " << size->width << "x" << size->height;
}

// Or with std::visit for pattern matching:
std::visit([](const auto& data) {
    using T = std::decay_t<decltype(data)>;
    if constexpr (std::is_same_v<T, laya::window_event_data_size>) {
        std::cout << "Size: " << data.width << "x" << data.height;
    }
}, win_event->data);
```

## Adding New Examples

When creating a new example:

1. **Accept command-line arguments**: Examples should support `--ci` flag
2. **Implement CI mode**: Run fixed iterations and exit automatically
3. **Document the example**: Add to this README with feature descriptions
4. **Update workflows**: Ensure CI workflows run the example with `--ci`

Example template:

```cpp
int main(int argc, char* argv[]) {
    bool ci_mode = false;
    if (argc > 1 && std::string(argv[1]) == "--ci") {
        ci_mode = true;
        std::cout << "Running in CI mode (non-interactive)" << std::endl;
    }

    // ... setup code ...

    constexpr int ci_iterations = 5;
    int iterations = 0;
    bool running = true;

    while (running) {
        // ... main loop ...

        if (ci_mode) {
            ++iterations;
            if (iterations >= ci_iterations) {
                std::cout << "CI mode: completed " << iterations << " iterations" << std::endl;
                running = false;
            }
        }
    }

    return 0;
}
```

## Philosophy

Examples demonstrate idiomatic usage of laya features:

- Modern C++20 patterns
- Zero-cost abstractions
- Type safety with strong types
- STL-like conventions
- Clear separation of concerns

## Troubleshooting

**Example hangs in CI:**

- Ensure you're passing the `--ci` flag
- Verify the example properly checks `ci_mode` in its main loop
- Check that iteration counter increments correctly

**Window doesn't appear:**

- Some CI environments are headless and don't support window creation
- Use `Xvfb` or similar virtual display for headless Linux
- Examples should still function and exit cleanly even without a visible window
