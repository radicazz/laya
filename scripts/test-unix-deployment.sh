#!/bin/bash

# Test script for verifying laya deployment on Ubuntu systems
# This script helps verify that our shared library setup works on clean Ubuntu installations

set -e

echo "=== laya Unix Deployment Test ==="

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check system information
echo "System: $(uname -a)"
echo "Distribution: $(lsb_release -d 2>/dev/null | cut -f2 || echo 'Unknown')"
echo

# Check essential tools
echo "Checking essential tools..."
for tool in cmake gcc g++ make ninja; do
    if command_exists $tool; then
        echo "✓ $tool: $(which $tool)"
    else
        echo "✗ $tool: not found"
    fi
done
echo

# Check system dependencies our shared libraries might need
echo "Checking system runtime dependencies..."
dependencies=(
    "libc.so.6"
    "libdl.so.2"
    "libpthread.so.0"
    "libm.so.6"
    "libX11.so.6"
    "libGL.so.1"
    "libasound.so.2"
)

for dep in "${dependencies[@]}"; do
    if ldconfig -p | grep -q "$dep"; then
        echo "✓ $dep: available"
    else
        echo "⚠ $dep: not found (may be optional)"
    fi
done
echo

# Try to build the project
if [ -f "CMakeLists.txt" ]; then
    echo "Building laya..."

    # Clean previous build
    rm -rf build-test

    # Configure with shared libraries
    cmake -B build-test \
        -DCMAKE_BUILD_TYPE=Debug \
        -DLAYA_BUILD_SHARED=ON \
        -DLAYA_SDL_METHOD=submodule \
        -DLAYA_BUILD_EXAMPLES=ON \
        -DLAYA_BUILD_TESTS=ON

    # Build
    cmake --build build-test --parallel

    echo "✓ Build successful!"
    echo

    # Check shared library deployment
    echo "Checking shared library deployment..."

    executable="build-test/examples/hello_laya"
    if [ -f "$executable" ]; then
        echo "Executable: $executable"

        # List shared libraries in the same directory
        echo "Deployed libraries:"
        ls -la "$(dirname $executable)"/*.so* 2>/dev/null | awk '{print "  " $9 " (" $5 " bytes)"}' || echo "  No .so files found"

        # Check executable dependencies
        echo
        echo "Runtime dependencies (ldd):"
        if command_exists ldd; then
            ldd "$executable" | head -15 | sed 's/^/  /'
        else
            echo "  ldd not available"
        fi

        # Test execution
        echo
        echo "Testing execution..."
        if timeout 10s "$executable"; then
            echo "✓ Execution successful!"
        else
            echo "⚠ Execution failed or timed out"
        fi
    else
        echo "✗ Executable not found at $executable"
    fi

    # Test the tests
    echo
    echo "Running tests..."
    if [ -f "build-test/tests/laya_tests" ]; then
        if timeout 30s "build-test/tests/laya_tests"; then
            echo "✓ Tests passed!"
        else
            echo "⚠ Tests failed or timed out"
        fi
    else
        echo "✗ Test executable not found"
    fi

else
    echo "✗ CMakeLists.txt not found. Run this script from the laya project root."
fi

echo
echo "=== Test Complete ==="

# Provide recommendations based on results
echo
echo "Recommendations for deployment:"
echo "1. Distribute your executable along with all .so files from its directory"
echo "2. For minimal Ubuntu systems, install: sudo apt-get install libasound2 libx11-6 libgl1-mesa-glx"
echo "3. Our build uses vendored libraries (FreeType, HarfBuzz) to minimize system dependencies"
echo "4. RPATH is set to \$ORIGIN so libraries are found relative to the executable"
