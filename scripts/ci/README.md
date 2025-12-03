# CI Scripts

Reusable automation scripts for CI/CD workflows.

## Overview

These scripts extract common functionality from GitHub Actions workflows, making them:
- **Testable locally** - Run without GitHub Actions infrastructure
- **Reusable** - Share logic across different workflows
- **Maintainable** - Single source of truth for common operations
- **Documented** - Each script has comprehensive help text

## Scripts

### Environment Setup

#### `setup-xvfb.sh`
Setup headless X11 display server for GUI testing.

```bash
./scripts/ci/setup-xvfb.sh
./scripts/ci/setup-xvfb.sh --display :1 --resolution 1280x720x24
```

#### `cleanup-xvfb.sh`
Stop Xvfb display server.

```bash
./scripts/ci/cleanup-xvfb.sh
./scripts/ci/cleanup-xvfb.sh 12345  # Specific PID
```

#### `install-ubuntu-deps.sh`
Install Ubuntu dependencies.

```bash
./scripts/ci/install-ubuntu-deps.sh --build      # Build dependencies
./scripts/ci/install-ubuntu-deps.sh --runtime    # Runtime dependencies
./scripts/ci/install-ubuntu-deps.sh --all        # All dependencies
```

#### `install-compiler.sh`
Install specific compiler versions.

```bash
./scripts/ci/install-compiler.sh --gcc 13
./scripts/ci/install-compiler.sh --clang 18
./scripts/ci/install-compiler.sh --gcc 13 --with-coverage
```

### Library Management

#### `find-sdl3-lib.sh`
Locate SDL3 shared library and setup LD_LIBRARY_PATH.

```bash
eval "$(./scripts/ci/find-sdl3-lib.sh)"
eval "$(./scripts/ci/find-sdl3-lib.sh --dir build)"
```

**Note**: Must be evaluated or sourced to affect calling shell.

### Testing & Benchmarking

#### `run-tests.sh`
Run unit tests with various output formats.

```bash
./scripts/ci/run-tests.sh
./scripts/ci/run-tests.sh --junit junit-gcc.xml --console console-gcc.txt
./scripts/ci/run-tests.sh --output-dir my-results
```

#### `generate-coverage.sh`
Generate code coverage reports using lcov.

```bash
./scripts/ci/generate-coverage.sh
./scripts/ci/generate-coverage.sh --gcov gcov-13
./scripts/ci/generate-coverage.sh --no-html
```

#### `run-benchmarks-linux.sh`
Run benchmarks with JSON and console output.

```bash
./scripts/ci/run-benchmarks-linux.sh --compiler gcc --build-type Release
./scripts/ci/run-benchmarks-linux.sh --output-dir bench-results
```

## Usage in Workflows

Example workflow step using these scripts:

```yaml
- name: Setup environment
  run: ./scripts/ci/setup-xvfb.sh

- name: Install dependencies
  run: ./scripts/ci/install-ubuntu-deps.sh --runtime

- name: Install compiler
  run: ./scripts/ci/install-compiler.sh --gcc 13

- name: Run tests
  run: |
    eval "$(./scripts/ci/find-sdl3-lib.sh)"
    ./scripts/ci/run-tests.sh --junit junit-results.xml

- name: Cleanup
  if: always()
  run: ./scripts/ci/cleanup-xvfb.sh
```

## Design Principles

- **Single Responsibility**: Each script does one thing well
- **Self-Documenting**: Comprehensive help text (`--help`)
- **Error Handling**: Proper exit codes and error messages
- **GitHub Actions Integration**: Export environment variables when available
- **Local Testing**: Work outside CI/CD environments

## Style Guide

All scripts follow the project's [Scripting Style Guide](../STYLE.md):

- Shebang: `#!/usr/bin/env bash`
- Error handling: `set -euo pipefail`
- Help text: `-h` / `--help` flags
- Consistent naming: lowercase with hyphens
