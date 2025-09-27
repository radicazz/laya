# Roadmap

> [!IMPORTANT]
> This document is a work in progress and subject to change.

This document serves as a broad outline for the development and direction of this project. From initial project setup to eventual advanced features, it provides a structured plan to guide the project's growth and visualize progress.

---

## 🚀 Pre-Release

The initial focus is on establishing a solid foundation for the project. This includes setting up the repository, build system, continuous integration (CI), and testing framework. The goal is to create a reliable environment that supports efficient development and collaboration.

### 1. Project Creation :green_circle:

- [x] Initialize repository with `LICENSE`, `README`, `CONTRIBUTING`, style guide, etc...
- [x] Add `.gitignore`, `.editorconfig`, `clang-format`, and other config files
- [x] Create initial `docs/` structure with some basic documentation

### 2. Build System :orange_circle:

- [ ] Add `CMakeLists.txt` with out-of-source build support  
- [ ] Add the ability to configure SDL3 as submodule or dependency
- [ ] Add minimal example that links against SDL3 for basic build validation

### 3. Testing Framework :red_circle:

- [ ] Add doctest (or Catch2/GoogleTest) [TBD]
- [ ] Integrate testing into CMake builds
- [ ] Add initial unit tests for example code

### 4. Continuous Integration :red_circle:

- [ ] Add GitHub Actions workflows (Linux, Windows, macOS)  
- [ ] Build → run example in pipeline  
- [ ] Cache dependencies for faster CI

### 5. Planning & Design :red_circle:

- [ ] Document SDL3 components and features to wrap
- [ ] Research and identify C++20 features to leverage
- [ ] Establish design principles and guidelines
- [ ] Begin adding implementation plans

---

## 🛠️ Unstable Release

With the foundation laid and a clear plan in place, the focus shifts to actual development. This phase involves implementing the core functionality, adhering to the defined style and design principles, and continuously testing and refining the code as we prepare for the first stable release.

### 1. Handle Initialization & Shutdown :red_circle:

> v0.1.0

- [ ] Implement `laya::init()` and `laya::quit()`
- [ ] Add `std::format` support for error messages
- [ ] Add basic error handling and reporting

### 2. Handle Window Management :red_circle:

> v0.2.0

- [ ] Implement `laya::createWindow()` and `laya::destroyWindow()`
- [ ] Add basic window event handling (resize, close)

---

> [!NOTE]
> Additional content to be determined at a later date...

---

## 🔄 Stable Releases

> [!NOTE]
> Additional content to be determined at a later date...

---

## ✅ Progress Tracking

For now this document will serve as a high-level *living checklist* of tasks to accomplish.
