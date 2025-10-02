# laya Examples

This directory contains example applications demonstrating various features of the laya library.

## Running Examples

### Interactive Mode (Default)

Run examples normally for interactive demonstrations:

```bash
# Build examples
cmake --build build --target laya_examples_events

# Run interactively
./build/examples/events/laya_examples_events
```

Examples will wait for user input (keyboard, mouse, window events) and can be closed by:

- Pressing **ESC**
- Clicking the window close button
- Sending a quit signal

### CI Mode (Non-Interactive)

For automated testing in CI/CD environments where no user interaction is possible, use the `--ci` flag:

```bash
./build/examples/events/laya_examples_events --ci
```

**CI Mode Behavior:**

- Runs for a fixed number of iterations (typically 5)
- Automatically exits without waiting for input
- Prints completion status
- Suitable for automated testing pipelines

## Available Examples

### `events/`

Demonstrates event polling mechanisms:

- **event_view**: Zero-allocation lazy event polling
- **event_range**: Stored events with multi-pass iteration

**Features demonstrated:**

- Type-safe event handling with `std::visit`
- Window creation and management
- Keyboard and mouse event processing
- Two different event polling strategies

**Interactive controls:**

- ESC: Exit demo
- Mouse clicks: Logged to console
- Key presses: Logged to console

## Adding New Examples

When creating new examples that use event loops or wait for user input:

1. **Accept command-line arguments:**

   ```cpp
   int main(int argc, char* argv[]) {
       bool ci_mode = (argc > 1 && std::string(argv[1]) == "--ci");
       // ...
   }
   ```

2. **Implement timeout/iteration limit for CI mode:**

   ```cpp
   int iterations = 0;
   constexpr int ci_max_iterations = 5;

   while (running) {
       // Normal event loop logic

       if (ci_mode) {
           if (++iterations >= ci_max_iterations) {
               std::cout << "CI mode: completed" << std::endl;
               break;
           }
       }
   }
   ```

3. **Update CI workflows** in `.github/workflows/` to run with `--ci` flag:

   ```yaml
   - name: Run examples
     run: |
       ./build/examples/your_example/your_example --ci
   ```

## Philosophy

Examples should:

- ✅ Be **self-contained** and easy to understand
- ✅ Demonstrate **one concept** clearly
- ✅ Include **comments** explaining key concepts
- ✅ Work in **both interactive and CI modes**
- ✅ Follow the project's **coding standards**
- ✅ Be **minimal** - show the feature, not everything

## Building

Examples are built as part of the normal build process:

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

Individual example targets:

- `laya_examples_events` - Event polling demonstration

## Troubleshooting

**Example hangs in CI:**

- Ensure you're running with `--ci` flag
- Check that the example implements CI mode properly
- Verify SDL can initialize in headless mode

**Window doesn't appear:**

- Check that SDL3 video subsystem initialized correctly
- On Linux, ensure X11 or Wayland is available
- In CI, window won't be visible (this is expected)

**Events not working:**

- Interactive mode: Ensure window has focus
- CI mode: Examples generate synthetic events or run for fixed iterations
- Check SDL initialization succeeded

## License

All examples are provided under the same license as the laya library. See [LICENSE.txt](../LICENSE.txt) for details.
