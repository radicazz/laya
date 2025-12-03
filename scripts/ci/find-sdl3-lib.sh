#!/usr/bin/env bash
###############################################################################
# Script Name: find-sdl3-lib.sh
# Description: Locate SDL3 shared library and setup LD_LIBRARY_PATH
# Usage      : eval "$(./scripts/ci/find-sdl3-lib.sh)"
#              source <(./scripts/ci/find-sdl3-lib.sh)
# Examples   : eval "$(./scripts/ci/find-sdl3-lib.sh)"
#              eval "$(./scripts/ci/find-sdl3-lib.sh --dir build)"
# Dependencies: find
# Notes      : - Searches for libSDL3.so.0 in specified directory
#              - Exports LD_LIBRARY_PATH for the calling shell
#              - Designed for CI/CD artifact environments
###############################################################################

set -euo pipefail
IFS=$'\n\t'

SEARCH_DIR="build"

###############################################################################
# Usage Information
###############################################################################
usage() {
    cat << EOF
Usage: eval "\$(${0##*/} [OPTIONS])"

Locate SDL3 shared library and setup LD_LIBRARY_PATH.

OPTIONS:
    -h, --help              Show this help message
    -d, --dir DIRECTORY     Search directory (default: build)

EXAMPLES:
    eval "\$(${0##*/})"
    eval "\$(${0##*/} --dir build)"
    source <(${0##*/})

NOTES:
    - Must be evaluated or sourced to affect calling shell
    - Searches for libSDL3.so.0 recursively
    - Appends to existing LD_LIBRARY_PATH
    - Exports LD_LIBRARY_PATH and SDL3_LIB_PATH
EOF
    exit 0
}

###############################################################################
# Argument Parsing
###############################################################################
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -d|--dir)
            SEARCH_DIR="$2"
            shift 2
            ;;
        *)
            echo "Error: Unknown option: $1" >&2
            usage
            ;;
    esac
done

###############################################################################
# Main
###############################################################################

# Find SDL3 library - search for .so.0 or versioned .so.0.* files
SDL3_LIB_PATH=$(find "$SEARCH_DIR" -type f \( -name "libSDL3.so.0" -o -name "libSDL3.so.0.*" \) -exec dirname {} \; 2>/dev/null | head -1)

if [[ -z "$SDL3_LIB_PATH" ]]; then
    echo "Error: libSDL3.so.0 or libSDL3.so.0.* not found in ${SEARCH_DIR}" >&2
    echo "Available SDL3 files:" >&2
    find "$SEARCH_DIR" -name "libSDL3*" -type f 2>/dev/null | while read -r file; do
        echo "  $file" >&2
    done
    exit 1
fi

# Output export statements for eval
cat << EOF
export SDL3_LIB_PATH="${SDL3_LIB_PATH}"
export LD_LIBRARY_PATH="${SDL3_LIB_PATH}:\${LD_LIBRARY_PATH:-}"
echo "✓ SDL3 library found: ${SDL3_LIB_PATH}" >&2
EOF

# Also export to GitHub Actions environment if available
if [[ -n "${GITHUB_ENV:-}" ]]; then
    echo "SDL3_LIB_PATH=${SDL3_LIB_PATH}" >> "$GITHUB_ENV"
    echo "LD_LIBRARY_PATH=${SDL3_LIB_PATH}:${LD_LIBRARY_PATH:-}" >> "$GITHUB_ENV"
fi
