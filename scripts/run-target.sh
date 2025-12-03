#!/usr/bin/env bash
# Script to build and run a specific CMake target
# Usage: ./scripts/run-target.sh <target_name> [args...]
# Example: ./scripts/run-target.sh example_logging --ci

set -euo pipefail

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Source color utility
# shellcheck source=scripts/print-color.sh
. "${SCRIPT_DIR}/print-color.sh"

# Check if target name provided
if [ $# -eq 0 ]; then
    error "No target specified"
    echo ""
    echo "Usage: $0 <target_name> [args...]"
    echo ""
    echo "Examples:"
    echo "  $0 example_logging --ci"
    echo "  $0 laya_tests_unit"
    echo "  $0 laya_tests_benchmark --test-suite=benchmark"
    exit 1
fi

TARGET_NAME="$1"
shift  # Remove target name from arguments

BUILD_DIR="$PROJECT_ROOT/build"

# Check if build directory exists, if not configure
if [ ! -d "$BUILD_DIR" ]; then
    info "Build directory not found, configuring..."
    if cmake -B "$BUILD_DIR" -S "$PROJECT_ROOT" > /dev/null 2>&1; then
        success "Configuration successful"
    else
        error "Configuration failed"
        exit 1
    fi
fi

# Build the target
info "Building target: $TARGET_NAME"
if cmake --build "$BUILD_DIR" --target "$TARGET_NAME" -j"$(nproc)" 2>&1 | \
    grep -E "Built target|Building|Linking|error|warning" || true; then
    success "Build successful"
else
    error "Build failed for target: $TARGET_NAME"
    exit 1
fi

echo ""

# Find the executable
# Common locations based on target naming conventions
EXECUTABLE=""

# Check for examples
if [[ "$TARGET_NAME" == example_* ]]; then
    EXECUTABLE="$BUILD_DIR/examples/${TARGET_NAME#example_}/$TARGET_NAME"
fi

# Check for test executables
if [[ "$TARGET_NAME" == laya_tests_* ]]; then
    EXECUTABLE="$BUILD_DIR/tests/$TARGET_NAME"
fi

# Check for renamed example targets
if [[ "$TARGET_NAME" == laya_examples_* ]]; then
    EXECUTABLE="$BUILD_DIR/examples/${TARGET_NAME#laya_examples_}/$TARGET_NAME"
fi

# Fallback: search for the executable in build directory
if [ -z "$EXECUTABLE" ] || [ ! -f "$EXECUTABLE" ]; then
    FOUND=$(find "$BUILD_DIR" -type f -name "$TARGET_NAME" -executable 2>/dev/null | head -n 1)
    if [ -n "$FOUND" ]; then
        EXECUTABLE="$FOUND"
    fi
fi

# Check if executable was found
if [ -z "$EXECUTABLE" ] || [ ! -f "$EXECUTABLE" ]; then
    error "Executable not found for target: $TARGET_NAME"
    echo ""
    warn "Searched locations:"
    echo "  - $BUILD_DIR/examples/*/$ TARGET_NAME"
    echo "  - $BUILD_DIR/tests/$TARGET_NAME"
    echo "  - $BUILD_DIR/**/$TARGET_NAME"
    exit 1
fi

# Run the executable with any additional arguments
info "Running: $EXECUTABLE $*"
printf '%b========================================%b\n' "$CYAN" "$RESET"

"$EXECUTABLE" "$@"

RESULT=$?

printf '%b========================================%b\n' "$CYAN" "$RESET"
echo ""

if [ $RESULT -eq 0 ]; then
    success "Target '$TARGET_NAME' completed successfully"
else
    error "Target '$TARGET_NAME' failed (exit code: $RESULT)"
    exit "$RESULT"
fi
