#!/usr/bin/env bash
###############################################################################
# Script Name: run-benchmarks.sh
# Description: Run Laya benchmarks with various output formats
# Usage      : ./scripts/ci/run-benchmarks.sh [OPTIONS]
# Examples   : ./scripts/ci/run-benchmarks.sh
#              ./scripts/ci/run-benchmarks.sh --compiler gcc
#              ./scripts/ci/run-benchmarks.sh --output-dir bench-results
# Dependencies: laya_tests_benchmark binary
# Notes      : - Automatically finds and sets SDL3 library path
#              - Generates both JSON and console output
###############################################################################

set -euo pipefail
IFS=$'\n\t'

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Default values
BENCH_BINARY="./build/tests/laya_tests_benchmark"
OUTPUT_DIR="benchmark-results"
COMPILER="unknown"
BUILD_TYPE="Release"

###############################################################################
# Usage Information
###############################################################################
usage() {
    cat << EOF
Usage: ${0##*/} [OPTIONS]

Run Laya benchmarks with JSON and console reporters.

OPTIONS:
    -h, --help                  Show this help message
    -b, --binary PATH           Path to benchmark binary (default: ./build/tests/laya_tests_benchmark)
    -o, --output-dir DIR        Output directory (default: benchmark-results)
    -c, --compiler NAME         Compiler name for output files (e.g., gcc, clang, msvc)
    -t, --build-type TYPE       Build type (default: Release)
    --no-sdl-search             Skip automatic SDL3 library search

EXAMPLES:
    ${0##*/}
    ${0##*/} --compiler gcc --build-type Release
    ${0##*/} --output-dir my-benchmarks

NOTES:
    - Automatically searches for SDL3 library and sets LD_LIBRARY_PATH
    - Generates JSON output for programmatic analysis
    - Generates console output for human readability
    - Creates metadata file with build information
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
            BENCH_BINARY="$2"
            shift 2
            ;;
        -o|--output-dir)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -c|--compiler)
            COMPILER="$2"
            shift 2
            ;;
        -t|--build-type)
            BUILD_TYPE="$2"
            shift 2
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
        echo "Warning: SDL3 library not found, benchmarks may fail" >&2
    fi
fi

# Check benchmark binary exists
if [[ ! -x "$BENCH_BINARY" ]]; then
    echo "Error: Benchmark binary not found or not executable: $BENCH_BINARY" >&2
    exit 1
fi

# Construct output filenames
BUILD_TYPE_LOWER=$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')
JSON_FILE="benchmarks-${COMPILER}-${BUILD_TYPE_LOWER}.json"
CONSOLE_FILE="benchmarks-${COMPILER}-${BUILD_TYPE_LOWER}.txt"
METADATA_FILE="metadata-${COMPILER}.txt"

# Run benchmarks with JSON output
echo "Running benchmarks with JSON reporter..."
"$BENCH_BINARY" \
    --test-suite=benchmark \
    --reporters=json \
    --out="${OUTPUT_DIR}/${JSON_FILE}" || true

echo "✓ JSON results: ${OUTPUT_DIR}/${JSON_FILE}"

# Run benchmarks with console output
echo "Running benchmarks with console reporter..."
"$BENCH_BINARY" \
    --test-suite=benchmark \
    --reporters=console | tee "${OUTPUT_DIR}/${CONSOLE_FILE}" || true

echo "✓ Console results: ${OUTPUT_DIR}/${CONSOLE_FILE}"

# Generate metadata
cat > "${OUTPUT_DIR}/${METADATA_FILE}" << EOF
Compiler: ${COMPILER}
Build Type: ${BUILD_TYPE}
Platform: Linux ($(uname -s) $(uname -m))
Timestamp: $(date -u +"%Y-%m-%dT%H:%M:%SZ")
Commit: ${GITHUB_SHA:-unknown}
Branch: ${GITHUB_REF_NAME:-unknown}
EOF

echo "✓ Metadata: ${OUTPUT_DIR}/${METADATA_FILE}"
echo "✓ Benchmarks completed successfully"
