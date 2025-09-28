<!-- Temporary disable markdownlint for specific rules to allow inline html -->
<!-- markdownlint-disable MD033 MD041 -->

<div align="center">
  <image alt="logo" src="docs/assets/logo_cropped.png" width="300"/>
</div>

<div align="center">
    <image alt="C++ 20 Badge" src="https://img.shields.io/badge/C%2B%2B-20-064F8C?logo=cplusplus&logoColor=white"/>
    <image alt="SDL3 Badge" src="https://img.shields.io/badge/SDL-3-064F8C?logo=files&logoColor=white"/>
    <image alt="CMake Badge" src="https://img.shields.io/badge/CMake-3.21-064F8C?logo=cmake&logoColor=white"/>
    <image alt="License: MIT" src="https://img.shields.io/github/license/radicazz/laya?color=orange&logo=open-source-initiative&logoColor=white"/>
</div>

<div align="center">
    <image alt="GitHub Last Commit" src="https://img.shields.io/github/last-commit/radicazz/laya?logo=github&logoColor=white"/>
    <image alt="Windows Build Status" src="https://github.com/radicazz/laya/actions/workflows/build_windows.yml/badge.svg?branch="/>
    <image alt="Linux Build Status" src="https://github.com/radicazz/laya/actions/workflows/build_linux.yml/badge.svg?branch="/>
</div>

<div align="center">
    <p>
        <a href="examples">Examples</a> •
        <a href="docs">Getting Started</a> •
        <a href="https://github.com/radicazz/laya/releases">Releases</a> •
        <a href="docs/design/roadmap.md">Roadmap</a>
    </p>
</div>

<!--
<div align="center">
    <image alt="GitHub Repo Size" src="https://img.shields.io/github/repo-size/radicazz/laya?logo=github&logoColor=white"/>
    <image alt="License: MIT" src="https://img.shields.io/github/license/radicazz/laya?color=orange&logo=open-source-initiative&logoColor=white"/>
    <image alt="GitHub Issues" src="https://img.shields.io/github/issues/radicazz/laya?logo=github&logoColor=white"/>
</div>
-->

<div align="center">
    <p>An intuitive wrapper for seamless integration of the <a href="https://github.com/libsdl-org/SDL">SDL</a> library in modern C++ projects.</p>
</div>

<!--
<div align="center">
    <p>
    The idea behind <i>laya</i> is to provide a clean, type-safe and idiomatic C++ interface for SDL and its extensions that feels natural while preserving the full power and flexibility of the underlying C library.
    </p>
</div>
-->

<!-- markdownlint-enable MD033 MD041 -->

# About

The idea behind *laya* is to provide a clean, type-safe and idiomatic C++ interface for SDL3 and its extensions that feels natural while preserving the full power and flexibility of the underlying C library.

## Features

- **Performance**: Minimal overhead compared to using SDL3 directly, ensuring that applications remain fast and responsive.
- **Completeness**: Full coverage (long-term goal) of SDL3 features, ensuring that no functionality is lost in translation.
- **Ergonomics**: A clean, intuitive API that leverages C++20 features to reduce boilerplate and improve code clarity.
- **Portability**: The library should support all platforms that compile C++20.
- **Safety**: Use strong typing, RAII, and other modern C++ idioms to minimize common pitfalls like resource leaks and undefined behavior.
- **Flexibility**: Allow users to choose between linking against a system-installed SDL or using a vendored version provided by *laya*.

## Contributing

If you are interested in contributing, please see [CONTRIBUTING.md](CONTRIBUTING.md) for some guidelines and best practices.

## License

This project falls under the MIT License. See [LICENSE.txt](LICENSE.txt) for details.
