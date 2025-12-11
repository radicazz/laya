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

## Style Guidelines

Code style is enforced via `.clang-format` and a pre-commit hook for automatic formatting. For detailed style guidelines including naming conventions, class organization, include order, and C++ best practices, see [STYLE.md](STYLE.md).

---

## License

By contributing, you agree that your contributions will be licensed under the MIT License, the same as the rest of the project.
