#!/usr/bin/env bash
###############################################################################
# Script Name: run-examples-headless.sh
# Description: Run examples and tests in headless environment using Xvfb
# Usage      : ./scripts/run-examples-headless.sh [OPTIONS]
# Examples   : ./scripts/run-examples-headless.sh
#              ./scripts/run-examples-headless.sh --examples-only
#              ./scripts/run-examples-headless.sh --tests-only
#              ./scripts/run-examples-headless.sh --display :1 --ci-iterations 10
# Dependencies: Xvfb, xdpyinfo, ctest, example binaries
# Notes      : - Automatically sets up and cleans up Xvfb
#              - Uses trap to ensure cleanup runs even on errors
#              - Runs examples with --ci flag for non-interactive execution
###############################################################################

set -euo pipefail
IFS=$'\n\t'

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default values
DISPLAY_NUM=":99"
CI_ITERATIONS="5"
RUN_TESTS=true
RUN_EXAMPLES=true
BUILD_DIR="${PROJECT_ROOT}/build"
XVFB_PID=""

###############################################################################
# Color Output Helpers
###############################################################################

# Detect TTY and support colors
if [[ -t 1 ]]; then
    RED="\e[31m"
    GREEN="\e[32m"
    YELLOW="\e[33m"
    CYAN="\e[36m"
    RESET="\e[0m"
else
    RED=""
    GREEN=""
    YELLOW=""
    CYAN=""
    RESET=""
fi

info() { printf "%b[INFO] %s%b\n" "$CYAN" "$*" "$RESET"; }
success() { printf "%b[SUCCESS] %s%b\n" "$GREEN" "$*" "$RESET"; }
warn() { printf "%b[WARN] %s%b\n" "$YELLOW" "$*" "$RESET"; }
error() { printf "%b[ERROR] %s%b\n" "$RED" "$*" "$RESET"; }

###############################################################################
# Cleanup Handler
###############################################################################

cleanup() {
    if [[ -n "$XVFB_PID" ]]; then
        info "Cleaning up Xvfb (PID: $XVFB_PID)..."
        if sudo kill "$XVFB_PID" 2>/dev/null; then
            success "Xvfb stopped successfully"
        else
            warn "Xvfb process may have already exited"
        fi
    fi
}

# Register cleanup to run on exit
trap cleanup EXIT

###############################################################################
# Usage Information
###############################################################################

usage() {
    cat << EOF
Usage: ${0##*/} [OPTIONS]

Run Laya examples and tests in a headless environment using Xvfb.

OPTIONS:
    -h, --help              Show this help message
    -e, --examples-only     Run examples only (skip CTest)
    -t, --tests-only        Run CTest only (skip examples)
    -d, --display NUM       Xvfb display number (default: :99)
    -i, --ci-iterations N   Number of iterations for examples (default: 5)
    -b, --build-dir DIR     Build directory (default: PROJECT_ROOT/build)

EXAMPLES:
    ${0##*/}
    ${0##*/} --examples-only
    ${0##*/} --tests-only
    ${0##*/} --display :1 --ci-iterations 10
    ${0##*/} --build-dir ./my-build

WORKFLOW:
    1. Start Xvfb virtual display server
    2. Export DISPLAY environment variable
    3. Run CTest tests (if enabled)
    4. Run examples with --ci flag (if enabled)
    5. Cleanup Xvfb (always runs via trap)

NOTES:
    - Requires Xvfb and xdpyinfo to be installed
    - Examples must be built with --ci flag support
    - Cleanup runs even if script fails or is interrupted
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
        -e|--examples-only)
            RUN_TESTS=false
            shift
            ;;
        -t|--tests-only)
            RUN_EXAMPLES=false
            shift
            ;;
        -d|--display)
            DISPLAY_NUM="$2"
            shift 2
            ;;
        -i|--ci-iterations)
            CI_ITERATIONS="$2"
            shift 2
            ;;
        -b|--build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        *)
            error "Unknown option: $1"
            usage
            ;;
    esac
done

###############################################################################
# Main
###############################################################################

info "Starting headless testing environment..."
echo "  Display: ${DISPLAY_NUM}"
echo "  Build directory: ${BUILD_DIR}"
echo "  Run tests: ${RUN_TESTS}"
echo "  Run examples: ${RUN_EXAMPLES}"
echo ""

# Verify build directory exists
if [[ ! -d "$BUILD_DIR" ]]; then
    error "Build directory not found: $BUILD_DIR"
    exit 1
fi

###############################################################################
# Step 1: Setup Xvfb
###############################################################################

info "Setting up Xvfb virtual display..."

# Check if Xvfb is available
if ! command -v Xvfb &> /dev/null; then
    error "Xvfb not found. Please install it first:"
    echo "  Ubuntu/Debian: sudo apt-get install xvfb xdpyinfo"
    exit 1
fi

# Start Xvfb in background
sudo Xvfb "${DISPLAY_NUM}" -screen 0 1920x1080x24 -ac +extension GLX +render -noreset > /dev/null 2>&1 &
XVFB_PID=$!

# Export DISPLAY for this script and subprocesses
export DISPLAY="${DISPLAY_NUM}"

# Wait for Xvfb to start
sleep 3

# Verify display is working
if ! xdpyinfo -display "${DISPLAY_NUM}" >/dev/null 2>&1; then
    error "Xvfb failed to start on display ${DISPLAY_NUM}"
    exit 1
fi

success "Virtual display ${DISPLAY_NUM} ready (PID: ${XVFB_PID})"
echo ""

###############################################################################
# Step 2: Run CTest (if enabled)
###############################################################################

if [[ "$RUN_TESTS" == true ]]; then
    info "Running CTest tests..."

    if ! command -v ctest &> /dev/null; then
        warn "CTest not found, skipping tests"
    else
        cd "$BUILD_DIR"

        if ctest --output-on-failure; then
            success "All CTest tests passed"
        else
            error "Some CTest tests failed"
            exit 1
        fi

        cd "$PROJECT_ROOT"
    fi
    echo ""
fi

###############################################################################
# Step 3: Run Examples (if enabled)
###############################################################################

if [[ "$RUN_EXAMPLES" == true ]]; then
    info "Running examples with --ci flag..."

    EXAMPLES_DIR="${BUILD_DIR}/examples"

    if [[ ! -d "$EXAMPLES_DIR" ]]; then
        warn "Examples directory not found: $EXAMPLES_DIR"
    else
        # Find all example executables
        EXAMPLE_BINARIES=$(find "$EXAMPLES_DIR" -type f -executable -name "laya_examples_*" | sort)

        if [[ -z "$EXAMPLE_BINARIES" ]]; then
            warn "No example binaries found in $EXAMPLES_DIR"
        else
            TOTAL=0
            PASSED=0
            FAILED=0

            while IFS= read -r example; do
                TOTAL=$((TOTAL + 1))
                EXAMPLE_NAME=$(basename "$example")

                info "Running: $EXAMPLE_NAME"

                if "$example" --ci; then
                    success "  ✓ $EXAMPLE_NAME passed"
                    PASSED=$((PASSED + 1))
                else
                    error "  ✗ $EXAMPLE_NAME failed"
                    FAILED=$((FAILED + 1))
                fi
            done <<< "$EXAMPLE_BINARIES"

            echo ""
            info "Examples Summary:"
            echo "  Total: $TOTAL"
            echo "  Passed: $PASSED"
            echo "  Failed: $FAILED"

            if [[ $FAILED -gt 0 ]]; then
                error "Some examples failed"
                exit 1
            fi

            success "All examples passed"
        fi
    fi
    echo ""
fi

###############################################################################
# Done
###############################################################################

success "Headless testing completed successfully!"
