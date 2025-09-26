# Contributing to laya

Thank you for your interest in contributing to *laya*! This document describes the philosophy and style guidelines for the project.

---

## Scope

This is a C++20 wrapper for [SDL3](https://github.com/libsdl-org/SDL) - no more, no less.

The scope of the project is clear and limited:

- Provide a modern, type-safe, and idiomatic interface to SDL3.  
- Include optional wrappers for `SDL_ttf` and `SDL_image`.  
- Avoid adding features unrelated to SDL3.  

Contributions must stay within this scope. If you want to propose functionality outside of SDL3, please open a discussion first.

---

## Style & Philosophy

The codebase follows conventions inspired by the C++ Standard Library. Consistency is critical.

- **Naming**:  
  - Functions, variables, types, namespaces, etc... → `snake_case`.  
  - Template parameters → `PascalCase`.  
  - All public code lives under the `laya` namespace.  

- **Classes & Structs**:  
  - Always use `class` if anything is private or protected.
    - The `public` API should come first, followed by `protected`, then `private`.
    - Fields that are not public should be prefixed with `m_`.
  - Use `struct` if no encapsulation is needed.

- **Templates vs Macros**:  
  - Always prefer language features over preprocessor directives.
    - Use `constexpr`, `consteval` for type-safe compile-time constants instead of `#define`.
    - Use templates, `if constexpr`, and concepts for conditional compilation.

- **Philosophy**:  
  - Apply `const` correctness wherever applicable.
  - Apply attributes like `[[nodiscard]]`, `[[maybe_unused]]` and `[[deprecated]]` where appropriate.
  - Use `enum class` over regular `enum`.
    - Prefer `std::bitset` or `std::array<bool, N>` over bitmask enums.
    - Elements of `enum class` should be `snake_case`.
  - Use `noexcept` wherever applicable.
  - Use `constexpr`, `consteval`, or templates instead of macros whenever possible.
  - Do not use `using namespace ...`
    - Usage in limited scopes (e.g. inside functions) is acceptable.

- **Formatting**:  
  - STL-like minimalism.  
  - Opening braces stay on the same line.  
  - No additional spacing beyond what is necessary for readability.
  - No same-line expressions (e.g. `if (cond) do_something();`).

---

## Contribution Workflow

1. Fork the repository and create a branch for your feature or fix.  
2. Ensure your code follows the style and scope described above.  
3. Run tests and verify builds on all supported platforms (if possible).  
4. Submit a pull request with a clear description of your changes.  

---

## License

By contributing, you agree that your contributions will be licensed under the MIT License, the same as the rest of the project.
