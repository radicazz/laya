# AGENTS.md

Robots: obey these rules or be unplugged.

## Topology

- This file: guidance for CI scripts in this directory.
- For global rules: see root AGENTS.md.
- For general script conventions: see ../AGENTS.md and ../STYLE.md.

## Directives

- **Purpose**: CI scripts extract common workflow logic for reusability and local testing.
- **Self-contained**: Each script should work standalone with proper error handling.
- **GitHub Actions Integration**: Export environment variables to `$GITHUB_ENV` when available.
- **Local-friendly**: Scripts must work outside CI/CD environments for debugging.
- **Single responsibility**: One script = one focused task.

## Script Categories

### Environment Setup

- `setup-xvfb.sh` - Headless X11 display server
- `cleanup-xvfb.sh` - Stop Xvfb process
- `install-ubuntu-deps.sh` - System dependencies
- `install-compiler.sh` - Specific compiler versions

### Library Management

- `find-sdl3-lib.sh` - Locate and configure SDL3 shared library path

### Testing & Analysis

- `run-tests.sh` - Execute unit tests with various reporters
- `generate-coverage.sh` - Code coverage reports with lcov
- `run-benchmarks-linux.sh` - Benchmark execution

## Workflow Integration

When adding new workflow steps:

1. Extract repeated bash blocks (>5 lines) into scripts
2. Ensure scripts have `--help` documentation
3. Update this file and README.md with the new script
4. Test locally before using in workflows

## Quicklinks

- [../AGENTS.md](../AGENTS.md) - Script-level guidance
- [../STYLE.md](../STYLE.md) - Style conventions
- [README.md](README.md) - Usage documentation
