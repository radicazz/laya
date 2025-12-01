# Scripts

Useful scripts for various tasks like building, testing, and more live in this directory.

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
