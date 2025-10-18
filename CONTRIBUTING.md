# Contributing to laya

Thank you for your interest in contributing to *laya*! This document describes the philosophy and style guidelines for the project.

---

## Contribution Workflow

1. Fork the repository and create a branch for your feature or fix.
2. Ensure your code follows the style and scope described above.
3. Run tests and verify builds on all supported platforms (if possible).
4. Submit a pull request with a clear description of your changes in English.

---

## Scope

This is (currently) a C++20 wrapper for [SDL3](https://github.com/libsdl-org/SDL) - no more, no less. This is meant to be used as a lightweight, efficient and low-level interface to SDL3, not a full application framework. Further abstractions and frameworks should be built on top of *laya*, by the library's consumer as they see fit.

The scope of the project is clear and limited:

- Provide a modern, type-safe, and idiomatic interface to SDL3 using the STL and modern features/practices.
- Include optional wrappers for `SDL_ttf` and `SDL_image`.
- Avoid adding features unrelated to SDL3 - if it's not in SDL3, it doesn't belong here.
- Attempt to provide lowest-cost abstractions wherever possible. Where not possible, provide multiple options (e.g., `event_range` vs `event_view`) for different use-cases.

Contributions must stay within this scope. If you want to propose functionality outside of SDL3, it must be justified and discussed first.

---

## Style & Philosophy

The codebase follows conventions inspired by the C++ Standard Library, for consistency and readability, enforced consistently across all files. Consistency is critical.

To assisst with this, a `.clang-format` file is provided for automatic formatting. Please use it, either via your IDE or command line. A pre-commit hook is also provided to automatically format code before committing.

### Naming Conventions

- **Files & directories** → `snake-case`
- **Functions, variables, types, namespaces** → `snake_case`
- **Template parameters** → `PascalCase`
- **All public code** lives under the `laya` namespace
- **Private member variables** are prefixed with `m_`
- **Macros** → `ALL_CAPS_WITH_UNDERSCORES`

Here's a quick reference:

```cpp
namespace laya {

// Types use snake_case
class window_manager;
struct window_event_data_position;
enum class window_event_type;

// Functions use snake_case
void initialize_subsystem();
[[nodiscard]] std::optional<window_event_data_size> get_size(const window_event& event) noexcept;

// Template parameters use PascalCase
template<typename EventType, typename Predicate>
auto filter_events(EventType&& events, Predicate&& pred);

}  // namespace laya
```

### Classes & Structs

- **Use `class`** if anything is private or protected
- **Use `struct`** if no encapsulation is needed
- **Member organization** within a class:
  1. Access specifiers in order: `public`, `protected`, `private`
  2. Within each section, group by: types, then methods, then variables
  3. Always explicitly specify access level (no implicit `private` in classes)
  4. Non-public member variables should be prefixed with `m_`
- **Inline class definitions are prohibited**; define classes at namespace scope and place every implementation in a translation unit.
- **No inline definitions in headers**: headers must only declare types and functions; implement them in the corresponding source file, using forward declarations as needed.
- **Header layout matters**: declare public APIs (concepts, aliases, function signatures) before type definitions and keep headers free of function bodies.

Example:

```cpp
class window {
public:
    // Public types first
    enum class flags : std::uint32_t {
        resizable = 0x01,
        fullscreen = 0x02
    };

    // Public methods second
    window(const char* title, std::pair<int, int> size, flags window_flags);
    ~window();

    [[nodiscard]] window_id id() const noexcept;
    [[nodiscard]] std::pair<int, int> size() const noexcept;

    void set_title(const char* title);
    void show();
    void hide();

    // Public variables last (if any - prefer getters/setters)

private:
    // Private types first (if any)

    // Private methods second
    void initialize();
    void cleanup() noexcept;

    // Private variables last
    SDL_Window* m_window;
    window_id m_id;
    std::string m_title;
};
```

For simple data structures with no encapsulation:

```cpp
struct window_event_data_position {
    std::int32_t x;
    std::int32_t y;
};
```

### Include Order

Includes should be organized in the following order with blank lines between groups:

1. **For source files (`.cpp`)**: The corresponding header file(s) first
2. **Standard library** headers (`<...>`)
3. **External library** headers (SDL3, doctest, etc.)
4. **Project headers** (relative paths `"..."`)

Example for a source file:

```cpp
// 1. Main header for this source file
#include <laya/events/event_types.hpp>

// 2. Standard library
#include <stdexcept>
#include <cstring>
#include <string>
#include <variant>

// 3. External libraries
#include <SDL3/SDL.h>

// 4. Project headers (if needed)
#include "internal_helper.hpp"
```

Example for a header file:

```cpp
#pragma once

// 1. Standard library
#include <variant>
#include <cstdint>
#include <optional>

// 2. External libraries (if needed)
// #include <SDL3/SDL_events.h>

// 3. Project headers
#include "../windows/window_id.hpp"
#include "event_polling.hpp"
```

### Comments & Documentation

- **File-level documentation**: Use `/// @file` and `/// @brief` at the top
- **Function/class documentation**: Use triple-slash `///` for Doxygen-style comments
- **Section separators**: Use block comments with `//` for visual organization
- **Inline comments**: Use `//` sparingly, prefer self-documenting code

Example:

```cpp
/// @file event_window.hpp
/// @brief Type-safe window events with std::variant for data fields
/// @date 2025-10-02

#pragma once

#include <variant>
#include <optional>

namespace laya {

// ============================================================================
// Window event data types
// ============================================================================

/// Position data for window move events
struct window_event_data_position {
    std::int32_t x;  ///< X coordinate
    std::int32_t y;  ///< Y coordinate
};

// ============================================================================
// Helper functions
// ============================================================================

/// Get position data from window event
/// @param event The window event to query
/// @return Position if event type is moved, std::nullopt otherwise
[[nodiscard]] std::optional<window_event_data_position>
get_position(const window_event& event) noexcept;

}  // namespace laya
```

### Templates vs Macros

Always prefer language features over preprocessor directives:

- Use `constexpr`, `consteval` for compile-time constants instead of `#define`
- Use templates, `if constexpr`, and concepts for conditional compilation
- **Always use `class` instead of `typename`** for template parameters

Example:

```cpp
// ❌ Bad - using preprocessor
#define MAX_EVENTS 100
#define IS_VALID(x) ((x) != nullptr)

// ✅ Good - using language features
constexpr int max_events = 100;
constexpr bool is_valid(const auto* ptr) noexcept { return ptr != nullptr; }

// ❌ Bad - using typename
template<typename T>
void process(typename T::value_type value);

// ✅ Good - using class
template<class T>
void process(typename T::value_type value);

// ✅ Also good - abbreviated function template syntax (C++20)
void process(const auto& value);
```

### Type Safety & Modern C++

- **Strong types**: Prefer strong type wrappers over raw primitives

  ```cpp
  // ❌ Avoid raw types for domain concepts
  std::uint32_t window_id;

  // ✅ Use strong type wrappers
  class window_id {
      std::uint32_t m_value;
  public:
      explicit constexpr window_id(std::uint32_t value) noexcept : m_value(value) {}
      [[nodiscard]] constexpr std::uint32_t value() const noexcept { return m_value; }
  };
  ```

- **Variant over union**: Use `std::variant` for type-safe discriminated unions

  ```cpp
  // ❌ Avoid C-style unions with manual type tracking
  struct event {
      enum type { moved, resized } event_type;
      union {
          struct { int x, y; } position;
          struct { int w, h; } size;
      };
  };

  // ✅ Use std::variant for type safety
  using window_event_data = std::variant<
      window_event_data_none,
      window_event_data_position,
      window_event_data_size
  >;
  ```

- **Attributes**: Apply `[[nodiscard]]`, `[[maybe_unused]]`, `[[deprecated]]` where appropriate

  ```cpp
  [[nodiscard]] std::optional<window_event_data_size>
  get_size(const window_event& event) noexcept;

  [[deprecated("Use get_size() instead")]]
  int get_width(const window_event& event);
  ```

- **Const correctness**: Apply `const` wherever possible

  ```cpp
  class window {
  public:
      [[nodiscard]] window_id id() const noexcept;  // const member function
      void set_title(const char* title);             // const parameter
  };
  ```

- **Enum class**: Always use `enum class` over regular `enum`
  - Elements should be `snake_case`
  - Prefer `std::bitset` or `std::array<bool, N>` over bitmask enums when possible

  ```cpp
  // ✅ Good
  enum class window_event_type {
      shown,
      hidden,
      moved,
      resized
  };

  // ❌ Avoid
  enum WindowEventType {
      SHOWN,
      HIDDEN
  };
  ```

- **Noexcept**: Mark functions `noexcept` when they cannot throw

  ```cpp
  [[nodiscard]] constexpr std::uint32_t value() const noexcept { return m_value; }
  ```

### Namespaces

- Do **not** use `using namespace ...` at global or namespace scope
- Usage in limited scopes (e.g., inside functions) is acceptable
- Always close namespace with a comment

Example:

```cpp
namespace laya {

void some_function() {
    // OK: limited scope
    using namespace std::chrono_literals;
    auto delay = 16ms;
}

}  // namespace laya

// ❌ Never do this at global scope
// using namespace std;
```

### Formatting

- **STL-like minimalism**: Clean, uncluttered code
- **Opening braces** stay on the same line (K&R style)
- **No same-line expressions**: Each statement on its own line
- **Spacing**: One blank line between function definitions, use blank lines to separate logical sections

Example:

```cpp
class event_view {
public:
    class iterator {
    public:
        // Methods grouped together
        iterator() noexcept = default;

        [[nodiscard]] const event& operator*() const noexcept {
            return m_current_event;
        }

        iterator& operator++() {
            if (!poll_next()) {
                m_has_event = false;
            }
            return *this;
        }

        [[nodiscard]] bool operator==(const iterator& other) const noexcept {
            return m_has_event == other.m_has_event;
        }

    private:
        bool poll_next();

        event m_current_event;
        bool m_has_event{false};
    };
};

// ❌ Bad - same-line expressions
if (condition) do_something();

// ✅ Good - separate lines
if (condition) {
    do_something();
}
```

---

## License

By contributing, you agree that your contributions will be licensed under the MIT License, the same as the rest of the project.
