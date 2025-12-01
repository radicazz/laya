# Scripts

Useful scripts for various tasks like building, testing, and more live in this directory.

## Style Guide

Please refer to the [Scripting Style Guide](./STYLE.md) for conventions and best practices when creating or modifying scripts in this project.

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
