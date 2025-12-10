# Scripts

Useful scripts for various tasks like building, testing, and more live in this directory.

## CI/CD Scripts

See [ci/README.md](./ci/README.md) for automation scripts used in GitHub Actions workflows. These scripts can also be run locally for development and debugging.

## Style Guide

Please refer to the [Scripting Style Guide](./STYLE.md) for conventions and best practices when creating or modifying scripts in this project.

## serve-docs.sh

Serves the MkDocs documentation site locally for development and preview.

```bash
# Serve docs on default port (8000)
./scripts/serve-docs.sh

# Serve on custom port
./scripts/serve-docs.sh --port 3000

# Serve on all network interfaces
./scripts/serve-docs.sh --host 0.0.0.0

# Custom host and port
./scripts/serve-docs.sh --host 0.0.0.0 --port 8080
```

The script will:

- Automatically install documentation dependencies via `uv`
- Start the MkDocs development server with live reload
- Watch for changes and auto-refresh the browser

## run-pre-commit.sh

Runs pre-commit hooks for the project using uv and the tools environment. This is a convenient wrapper around the pre-commit command referenced in the project's AGENTS.md file.

```bash
# Run all hooks on all files (default)
./scripts/run-pre-commit.sh

# Run hooks only on specific files
./scripts/run-pre-commit.sh --files src/laya/window.cpp

# Run hooks for push stage
./scripts/run-pre-commit.sh --hook-stage push

# Show diffs when hooks fail
./scripts/run-pre-commit.sh --show-diff-on-failure

# Get help and see all options
./scripts/run-pre-commit.sh --help
```

The script will:

- Execute `uv run --project tools --group dev pre-commit run` with provided arguments
- Default to `--all-files` when no arguments are given
- Provide colored output for better readability
- Pass through all pre-commit flags and options
- Exit with the same code as the underlying pre-commit command

## run-actions.sh

```bash
# List all workflows
./scripts/run-actions.sh --list

# Run static analysis workflow
./scripts/run-actions.sh --workflow static

# Dry run to see what would execute
./scripts/run-actions.sh --workflow build --dry-run

# Run specific job in test workflow
./scripts/run-actions.sh --workflow test --job test-gcc-debug
```
