#!/usr/bin/env bash
###############################################################################
# Script Name: generate-coverage.sh
# Description: Generate code coverage report using lcov
# Usage      : ./scripts/ci/generate-coverage.sh [OPTIONS]
# Examples   : ./scripts/ci/generate-coverage.sh
#              ./scripts/ci/generate-coverage.sh --gcov gcov-13
#              ./scripts/ci/generate-coverage.sh --output-dir coverage
# Dependencies: lcov, genhtml, gcov
# Notes      : - Requires test binary to be built with coverage flags
#              - Automatically runs tests before generating coverage
###############################################################################

set -euo pipefail
IFS=$'\n\t'

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Default values
TEST_BINARY="./build/tests/laya_tests_unit"
GCOV_TOOL="gcov"
OUTPUT_FILE="coverage.info"
HTML_DIR="coverage-html"
BUILD_DIR="."

###############################################################################
# Usage Information
###############################################################################
usage() {
    cat << EOF
Usage: ${0##*/} [OPTIONS]

Generate code coverage report for Laya tests.

OPTIONS:
    -h, --help                  Show this help message
    -b, --binary PATH           Path to test binary (default: ./build/tests/laya_tests_unit)
    -g, --gcov TOOL             gcov tool to use (default: gcov, e.g., gcov-13)
    -o, --output FILE           Output coverage file (default: coverage.info)
    -d, --html-dir DIR          HTML output directory (default: coverage-html)
    --build-dir DIR             Build directory to scan (default: .)
    --no-html                   Skip HTML report generation

EXAMPLES:
    ${0##*/}
    ${0##*/} --gcov gcov-13
    ${0##*/} --output my-coverage.info --html-dir html-report

NOTES:
    - Automatically searches for SDL3 library
    - Runs tests before generating coverage
    - Excludes system headers and third-party dependencies
    - Generates both .info file and HTML report
EOF
    exit 0
}

###############################################################################
# Argument Parsing
###############################################################################
GENERATE_HTML=true

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -b|--binary)
            TEST_BINARY="$2"
            shift 2
            ;;
        -g|--gcov)
            GCOV_TOOL="$2"
            shift 2
            ;;
        -o|--output)
            OUTPUT_FILE="$2"
            shift 2
            ;;
        -d|--html-dir)
            HTML_DIR="$2"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --no-html)
            GENERATE_HTML=false
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

# Setup SDL3 library path
echo "Searching for SDL3 library..."
if eval "$("$SCRIPT_DIR/find-sdl3-lib.sh")"; then
    echo "  LD_LIBRARY_PATH=${LD_LIBRARY_PATH}"
fi

# Check test binary exists
if [[ ! -x "$TEST_BINARY" ]]; then
    echo "Error: Test binary not found or not executable: $TEST_BINARY" >&2
    exit 1
fi

# Run tests to generate coverage data
echo "Running tests to generate coverage data..."
"$TEST_BINARY"
echo "✓ Tests completed"

# Generate coverage with lcov
echo "Capturing coverage data with ${GCOV_TOOL}..."
lcov --gcov-tool "$GCOV_TOOL" \
     --capture \
     --directory "$BUILD_DIR" \
     --output-file "$OUTPUT_FILE" || true

# Remove unwanted files from coverage
echo "Filtering coverage data..."
lcov --gcov-tool "$GCOV_TOOL" \
     --remove "$OUTPUT_FILE" \
     '/usr/*' \
     '*/build/_deps/*' \
     '*/tests/*' \
     --output-file "$OUTPUT_FILE" || true

echo "✓ Coverage data: $OUTPUT_FILE"

# Generate HTML report
if [[ "$GENERATE_HTML" == true ]]; then
    echo "Generating HTML coverage report..."
    mkdir -p "$HTML_DIR"
    genhtml "$OUTPUT_FILE" --output-directory "$HTML_DIR" || true
    echo "✓ HTML report: $HTML_DIR/index.html"
fi

# Display summary
echo ""
echo "Coverage Summary:"
lcov --list "$OUTPUT_FILE" || true
