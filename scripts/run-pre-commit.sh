#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

# run-pre-commit.sh
#
# Runs pre-commit hooks for the project using uv and the tools environment.
# By default runs all hooks on all files, but can pass additional flags to pre-commit.
#
# Usage:
#   ./scripts/run-pre-commit.sh                    # Run all hooks on all files
#   ./scripts/run-pre-commit.sh --files file.py   # Run hooks only on specific files
#   ./scripts/run-pre-commit.sh --hook-stage push # Run hooks for push stage
#   ./scripts/run-pre-commit.sh -h                # Show this help

# Color helpers for TTY output
if [[ -t 1 ]]; then
  RED="\e[31m"; GREEN="\e[32m"; YELLOW="\e[33m"; CYAN="\e[36m"; MAGENTA="\e[35m"; RESET="\e[0m"
else
  RED=""; GREEN=""; YELLOW=""; CYAN=""; MAGENTA=""; RESET=""
fi

info() { printf "%b[INFO] %s%b\n" "$CYAN" "$*" "$RESET"; }
success() { printf "%b[SUCCESS] %s%b\n" "$GREEN" "$*" "$RESET"; }
warn() { printf "%b[WARN] %s%b\n" "$YELLOW" "$*" "$RESET"; }
error() { printf "%b[ERROR] %s%b\n" "$RED" "$*" "$RESET"; }

usage() {
    cat << EOF
run-pre-commit.sh - Run pre-commit hooks for the laya project

SYNOPSIS
    ./scripts/run-pre-commit.sh [OPTIONS...]

DESCRIPTION
    Executes pre-commit hooks using uv and the tools environment. By default,
    runs all hooks on all files. Additional flags are passed through to the
    pre-commit command.

OPTIONS
    -h, --help          Show this help message

    All other options are passed directly to pre-commit. Common options include:
    --files FILE...     Run hooks only on specified files
    --hook-stage STAGE  Run hooks for specific stage (commit, push, etc.)
    --all-files         Run on all files (default when no args provided)
    --show-diff-on-failure  Show diff when hooks fail

EXAMPLES
    # Run all hooks on all files (default)
    ./scripts/run-pre-commit.sh

    # Run hooks only on specific files
    ./scripts/run-pre-commit.sh --files src/laya/window.cpp

    # Run hooks for push stage
    ./scripts/run-pre-commit.sh --hook-stage push

    # Show diffs when hooks fail
    ./scripts/run-pre-commit.sh --show-diff-on-failure

EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            usage
            exit 0
            ;;
        *)
            # All other arguments are passed to pre-commit
            break
            ;;
    esac
done

# If no arguments provided, default to --all-files
if [[ $# -eq 0 ]]; then
    set -- --all-files
fi

# Check if we're in the project root (look for pyproject.toml in tools/)
if [[ ! -f "tools/pyproject.toml" ]]; then
    error "This script must be run from the project root directory"
    error "Expected to find tools/pyproject.toml in the current directory"
    exit 1
fi

# Show what we're about to do
info "Running pre-commit hooks with: uv run --project tools --group dev pre-commit run $*"

# Execute pre-commit via uv
if uv run --project tools --group dev pre-commit run "$@"; then
    success "Pre-commit hooks completed successfully"
else
    exit_code=$?
    error "Pre-commit hooks failed (exit code: $exit_code)"
    exit $exit_code
fi
