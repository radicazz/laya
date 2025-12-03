#!/usr/bin/env bash
# Script to serve the MkDocs documentation site locally
# Usage: ./scripts/serve-docs.sh [--port PORT] [--host HOST]

set -euo pipefail

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
TOOLS_DIR="$PROJECT_ROOT/tools"

# Source color utility
# shellcheck source=scripts/print-color.sh
. "${SCRIPT_DIR}/print-color.sh"

# Default values
PORT=8000
HOST="127.0.0.1"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --port)
            PORT="$2"
            shift 2
            ;;
        --host)
            HOST="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [--port PORT] [--host HOST]"
            echo ""
            echo "Options:"
            echo "  --port PORT    Port to serve on (default: 8000)"
            echo "  --host HOST    Host to bind to (default: 127.0.0.1)"
            echo "  -h, --help     Show this help message"
            exit 0
            ;;
        *)
            error "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

printf '%b========================================%b\n' "$CYAN" "$RESET"
printf '%b  Laya Documentation Server%b\n' "$CYAN" "$RESET"
printf '%b========================================%b\n' "$CYAN" "$RESET"
echo ""

# Check if uv is installed
if ! command -v uv &> /dev/null; then
    error "uv is not installed"
    echo "Install uv: https://github.com/astral-sh/uv"
    exit 1
fi

# Check if tools directory exists
if [[ ! -d "$TOOLS_DIR" ]]; then
    error "Tools directory not found: $TOOLS_DIR"
    exit 1
fi

# Check if pyproject.toml exists
if [[ ! -f "$TOOLS_DIR/pyproject.toml" ]]; then
    error "pyproject.toml not found in tools directory"
    exit 1
fi

info "Using tools directory: $TOOLS_DIR"
info "Serving on: http://${HOST}:${PORT}"
echo ""

# Sync dependencies if needed
info "Ensuring documentation dependencies are installed..."
if uv sync --directory "$TOOLS_DIR" --group docs --quiet; then
    success "Dependencies ready"
else
    error "Failed to sync dependencies"
    exit 1
fi

echo ""
info "Starting MkDocs development server..."
info "Press Ctrl+C to stop"
echo ""

# Serve the documentation
# Using absolute path to mkdocs.yml since uv run changes to tools directory
exec uv run --directory "$TOOLS_DIR" mkdocs serve \
    --config-file "$PROJECT_ROOT/mkdocs.yml" \
    --dev-addr "${HOST}:${PORT}"
