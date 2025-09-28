#!/usr/bin/env bash
#===============================================================================
# File: unix-health-check.sh
# Description: Unix health-check helper for the laya project.
# - Verifies essential build tools and runtime libraries are present
# - Compares the local Ubuntu package set against CI's build list
# - Can optionally install missing CI build packages (uses sudo)
# - Non-destructive by default; interactive prompts can be suppressed
# License: MIT
#===============================================================================

# TODO:
# - Split this script into smaller components. (tools, packages, build, test, etc.)
# - Potentially make a windows equivalent.

set -euo pipefail

INSTALL_MISSING=0
ASSUME_YES=0

usage() {
    cat <<EOF
Usage: $(basename "$0") [options]

Options:
  --install-missing   Attempt to install any missing CI build packages (uses sudo)
  --assume-yes        When installing, don't prompt (pass -y to apt)
  -h, --help          Show this help message

This script helps validate a local Ubuntu environment matches the CI build
environment and can optionally install missing packages used by CI.
EOF
}

while [ $# -gt 0 ]; do
    case "$1" in
        --install-missing)
            INSTALL_MISSING=1
            shift
            ;;
        --assume-yes)
            ASSUME_YES=1
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            usage
            exit 2
            ;;
    esac
done

echo "=== laya Unix Health Check ==="

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Quick system info
echo "System: $(uname -a)"
echo "Distribution: $(lsb_release -d 2>/dev/null | cut -f2 || echo 'Unknown')"
echo

echo "Checking essential tools..."
for tool in cmake gcc g++ make ninja; do
    if command_exists $tool; then
        echo "✓ $tool: $(which $tool)"
    else
        echo "✗ $tool: not found"
    fi
done
echo

echo "Checking runtime dependencies..."
runtime_deps=(
    "libc.so.6"
    "libdl.so.2"
    "libpthread.so.0"
    "libm.so.6"
    "libX11.so.6"
    "libGL.so.1"
    "libasound.so.2"
)

for dep in "${runtime_deps[@]}"; do
    if ldconfig -p | grep -q "$dep"; then
        echo "✓ $dep: available"
    else
        echo "⚠ $dep: not found (may be optional)"
    fi
done
echo

# Ubuntu packages used in CI (build-time / -dev packages)
ubuntu_build_packages=(
    build-essential
    cmake
    ninja-build
    pkg-config
    libasound2-dev
    libpulse-dev
    libaudio-dev
    libjack-dev
    libsndio-dev
    libsamplerate0-dev
    libx11-dev
    libxext-dev
    libxrandr-dev
    libxcursor-dev
    libxfixes-dev
    libxi-dev
    libxss-dev
    libwayland-dev
    libxkbcommon-dev
    libdrm-dev
    libgl1-mesa-dev
    libgles2-mesa-dev
    libegl1-mesa-dev
    libdbus-1-dev
    libibus-1.0-dev
    libudev-dev
    fcitx-libs-dev
    libfreetype6-dev
    libharfbuzz-dev
    libpng-dev
    libjpeg-dev
    libtiff-dev
    zlib1g-dev
    xvfb
)

if command_exists apt-get && command_exists dpkg; then
    echo "Checking Ubuntu packages (as listed in CI)..."
    missing_pkgs=()
    for pkg in "${ubuntu_build_packages[@]}"; do
        if dpkg -s "$pkg" >/dev/null 2>&1; then
            echo "✓ $pkg: installed"
        else
            echo "✗ $pkg: not installed"
            missing_pkgs+=("$pkg")
        fi
    done
    echo

    if [ ${#missing_pkgs[@]} -ne 0 ]; then
        echo "Some CI build packages are missing: ${missing_pkgs[*]}"
        if [ $INSTALL_MISSING -eq 1 ]; then
            if ! command_exists sudo; then
                echo "Cannot install packages: sudo is not available on this system."
                exit 1
            fi

            if [ $ASSUME_YES -eq 0 ]; then
                echo
                echo "The script will run:"
                echo "  sudo apt-get update && sudo apt-get install -y ${missing_pkgs[*]}"
                read -r -p "Proceed to install the missing packages? [y/N] " answer
                case "$answer" in
                    [Yy]* ) ;;
                    * ) echo "Installation cancelled by user."; exit 1 ;;
                esac
            else
                echo "Installing missing packages (non-interactive)..."
            fi

            # Perform installation
            if [ $ASSUME_YES -eq 1 ]; then
                sudo apt-get update && sudo apt-get install -y "${missing_pkgs[@]}"
            else
                sudo apt-get update && sudo apt-get install -y "${missing_pkgs[@]}"
            fi

            echo "Package installation complete."
        else
            echo "To install missing packages run:"
            echo
            echo "  sudo apt-get update && sudo apt-get install -y ${missing_pkgs[*]}"
            echo
        fi
    else
        echo "All CI-listed Ubuntu packages appear to be installed."
        echo
    fi
else
    echo "apt/dpkg not available on this system; cannot check package installation status."
    echo
fi

# Compiler selection: if both gcc and clang are available, prompt the user
# to choose which to use for the local build. Prefer versioned names (gcc-11,
# clang-15) if present.
find_compiler() {
    for name in "$@"; do
        if command_exists "$name"; then
            echo "$name"
            return 0
        fi
    done
    return 1
}

detected_gcc=$(find_compiler gcc-11 gcc)
detected_clang=$(find_compiler clang-15 clang)

if [ -n "$detected_gcc" ] && [ -n "$detected_clang" ]; then
    if [ $ASSUME_YES -eq 1 ]; then
        # Non-interactive: prefer gcc by default
        chosen=gcc
    else
        echo "Both GCC and Clang compilers were detected on this system:"
        echo "  gcc -> $detected_gcc"
        echo "  clang -> $detected_clang"
        read -r -p "Which compiler would you like to use for the build? [gcc/clang] (default: gcc) " chosen
        chosen=${chosen:-gcc}
    fi
elif [ -n "$detected_gcc" ]; then
    chosen=gcc
elif [ -n "$detected_clang" ]; then
    chosen=clang
else
    chosen=""
fi

if [ -n "$chosen" ]; then
    if [ "$chosen" = "gcc" ]; then
        cc_cmd=$(find_compiler gcc-11 gcc)
        cxx_cmd=$(find_compiler g++-11 g++)
    else
        cc_cmd=$(find_compiler clang-15 clang)
        cxx_cmd=$(find_compiler clang++-15 clang++)
    fi

    if [ -n "$cc_cmd" ] && [ -n "$cxx_cmd" ]; then
        export CC="$cc_cmd"
        export CXX="$cxx_cmd"
        echo "Using compiler: $chosen (CC=$CC, CXX=$CXX)"
    else
        echo "Warning: selected compiler ($chosen) not found for both CC/CXX; leaving defaults to CMake." >&2
    fi
else
    echo "No supported compilers (gcc/clang) found; relying on system defaults." >&2
fi

if [ -f "CMakeLists.txt" ]; then
    echo "Building laya in build-test..."
    rm -rf build-test

    cmake -B build-test \
        -DCMAKE_BUILD_TYPE=Debug \
        -DLAYA_BUILD_SHARED=ON \
        -DLAYA_SDL_METHOD=submodule \
        -DLAYA_BUILD_EXAMPLES=ON \
        -DLAYA_BUILD_TESTS=ON

    cmake --build build-test --parallel

    echo "✓ Build successful"
    echo

    executable="build-test/examples/hello_laya"
    if [ -f "$executable" ]; then
        echo "Executable: $executable"
        echo "Deployed libraries:"
        ls -la "$(dirname $executable)"/*.so* 2>/dev/null | awk '{print "  " $9 " (" $5 " bytes)"}' || echo "  No .so files found"

        echo
        echo "Runtime dependencies (ldd):"
        if command_exists ldd; then
            ldd "$executable" | head -15 | sed 's/^/  /'
        else
            echo "  ldd not available"
        fi

        echo
        echo "Testing execution (timeout 10s)..."
        if timeout 10s "$executable"; then
            echo "✓ Execution successful"
        else
            echo "⚠ Execution failed or timed out"
        fi
    else
        echo "✗ Executable not found at $executable"
    fi

    echo
    echo "Running tests..."
    if [ -f "build-test/tests/laya_tests" ]; then
        if timeout 30s "build-test/tests/laya_tests"; then
            echo "✓ Tests passed"
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
echo "=== Health Check Complete ==="

echo
echo "Recommendations:"
echo "  1) Distribute executables with required .so files present in the same directory."
echo "  2) Minimal runtime packages: libasound2, libx11-6, libgl1-mesa-glx (install with apt)."
echo "  3) RPATH is set to \$ORIGIN so libraries are found relative to the executable."
echo "  4) CI installs additional -dev packages required to build from source; see .github/workflows/build_linux.yml."
