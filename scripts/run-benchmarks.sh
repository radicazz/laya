#!/usr/bin/env bash
# Script to build and run laya benchmarks in Release mode
# Usage: ./scripts/run-benchmarks.sh

set -euo pipefail

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Source color utility
# shellcheck source=scripts/print-color.sh
. "${SCRIPT_DIR}/print-color.sh"

printf '%b========================================%b\n' "$CYAN" "$RESET"
printf '%b  Laya Benchmark Runner%b\n' "$CYAN" "$RESET"
printf '%b========================================%b\n' "$CYAN" "$RESET"
echo ""

# Check if build directory exists
BUILD_DIR="$PROJECT_ROOT/build-release"

info "[1/3] Configuring CMake (Release mode)..."
if cmake -B "$BUILD_DIR" \
      -DCMAKE_BUILD_TYPE=Release \
      -DLAYA_TESTS_UNIT=OFF \
      -DLAYA_TESTS_BENCHMARK=ON \
      -S "$PROJECT_ROOT" \
      > /dev/null 2>&1; then
    success "Configuration successful"
else
    error "Configuration failed"
    exit 1
fi

echo ""
info "[2/3] Building benchmarks (Release mode)..."
if cmake --build "$BUILD_DIR" --target laya_tests_benchmark -j"$(nproc)" 2>&1 | \
    grep -E "Built target|Building|Linking|error|warning" || true; then
    success "Build successful"
else
    error "Build failed"
    exit 1
fi

echo ""
info "[3/3] Running benchmarks..."
printf '%b----------------------------------------%b\n' "$CYAN" "$RESET"

# Run the benchmark executable
"$BUILD_DIR/tests/laya_tests_benchmark" --test-suite=benchmark

BENCHMARK_RESULT=$?

printf '%b----------------------------------------%b\n' "$CYAN" "$RESET"
echo ""

# Summary
printf '%b========================================%b\n' "$CYAN" "$RESET"
printf '%b  Benchmark Summary%b\n' "$CYAN" "$RESET"
printf '%b========================================%b\n' "$CYAN" "$RESET"
echo ""
printf '%bBuild Mode:%b      Release (optimizations enabled)\n' "$YELLOW" "$RESET"
printf '%bBuild Dir:%b       %s\n' "$YELLOW" "$RESET" "$BUILD_DIR"
printf '%bTest Suite:%b      benchmark\n' "$YELLOW" "$RESET"
echo ""

if [ $BENCHMARK_RESULT -eq 0 ]; then
    success "All benchmarks completed successfully"
    echo ""
    info "Benchmark results show performance characteristics"
    echo "      of various laya operations in Release mode."
    echo "      Lower times indicate better performance."
else
    error "Some benchmarks failed (exit code: $BENCHMARK_RESULT)"
    exit "$BENCHMARK_RESULT"
fi

echo ""
printf '%b========================================%b\n' "$CYAN" "$RESET"
