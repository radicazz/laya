#!/usr/bin/env bash
###############################################################################
# Script Name: run-tests.sh
# Description: Run Laya unit tests with various output formats
# Usage      : ./scripts/ci/run-tests.sh [OPTIONS]
# Examples   : ./scripts/ci/run-tests.sh
#              ./scripts/ci/run-tests.sh --junit junit-results.xml
#              ./scripts/ci/run-tests.sh --console --output-dir results
# Dependencies: laya_tests_unit binary
# Notes      : - Automatically finds and sets SDL3 library path
#              - Creates output directory if it doesn't exist
###############################################################################

set -euo pipefail
IFS=$'\n\t'

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Default values
TEST_BINARY="./build/tests/laya_tests_unit"
OUTPUT_DIR="test-results"
JUNIT_FILE=""
CONSOLE_FILE=""
RUN_JUNIT=true
RUN_CONSOLE=true

###############################################################################
# Usage Information
###############################################################################
usage() {
    cat << EOF
Usage: ${0##*/} [OPTIONS]

Run Laya unit tests with various reporters.

OPTIONS:
    -h, --help                  Show this help message
    -b, --binary PATH           Path to test binary (default: ./build/tests/laya_tests_unit)
    -o, --output-dir DIR        Output directory (default: test-results)
    -j, --junit FILE            JUnit XML output filename
    -c, --console FILE          Console output filename
    --junit-only                Only generate JUnit XML output
    --console-only              Only generate console output
    --no-sdl-search             Skip automatic SDL3 library search

EXAMPLES:
    ${0##*/}
    ${0##*/} --junit junit-gcc.xml --console console-gcc.txt
    ${0##*/} --output-dir my-results
    ${0##*/} --console-only

NOTES:
    - Automatically searches for SDL3 library and sets LD_LIBRARY_PATH
    - Creates output directory if needed
    - Default JUnit file: junit-results.xml
    - Default console file: console-results.txt
EOF
    exit 0
}

###############################################################################
# Argument Parsing
###############################################################################
SDL_SEARCH=true

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -b|--binary)
            TEST_BINARY="$2"
            shift 2
            ;;
        -o|--output-dir)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -j|--junit)
            JUNIT_FILE="$2"
            shift 2
            ;;
        -c|--console)
            CONSOLE_FILE="$2"
            shift 2
            ;;
        --junit-only)
            RUN_CONSOLE=false
            shift
            ;;
        --console-only)
            RUN_JUNIT=false
            shift
            ;;
        --no-sdl-search)
            SDL_SEARCH=false
            shift
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

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Setup SDL3 library path if needed
if [[ "$SDL_SEARCH" == true ]]; then
    echo "Searching for SDL3 library..."
    if eval "$("$SCRIPT_DIR/find-sdl3-lib.sh")"; then
        echo "  LD_LIBRARY_PATH=${LD_LIBRARY_PATH}"
    else
        echo "Warning: SDL3 library not found, tests may fail" >&2
    fi
fi

# Check test binary exists
if [[ ! -x "$TEST_BINARY" ]]; then
    echo "Error: Test binary not found or not executable: $TEST_BINARY" >&2
    exit 1
fi

# Set default filenames if not provided
JUNIT_FILE="${JUNIT_FILE:-junit-results.xml}"
CONSOLE_FILE="${CONSOLE_FILE:-console-results.txt}"

# Run tests with JUnit output
if [[ "$RUN_JUNIT" == true ]]; then
    echo "Running tests with JUnit reporter..."
    "$TEST_BINARY" --reporters=junit --out="${OUTPUT_DIR}/${JUNIT_FILE}"
    echo "✓ JUnit results: ${OUTPUT_DIR}/${JUNIT_FILE}"
fi

# Run tests with console output
if [[ "$RUN_CONSOLE" == true ]]; then
    echo "Running tests with console reporter..."
    "$TEST_BINARY" --reporters=console | tee "${OUTPUT_DIR}/${CONSOLE_FILE}"
    echo "✓ Console results: ${OUTPUT_DIR}/${CONSOLE_FILE}"
fi

echo "✓ Tests completed successfully"
