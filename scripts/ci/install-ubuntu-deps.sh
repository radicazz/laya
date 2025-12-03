#!/usr/bin/env bash
###############################################################################
# Script Name: install-ubuntu-deps.sh
# Description: Install Ubuntu dependencies for building and testing
# Usage      : ./scripts/ci/install-ubuntu-deps.sh [OPTIONS]
# Examples   : ./scripts/ci/install-ubuntu-deps.sh --build
#              ./scripts/ci/install-ubuntu-deps.sh --runtime
#              ./scripts/ci/install-ubuntu-deps.sh --build --runtime
# Dependencies: apt-get
# Notes      : - Requires sudo privileges
#              - Updates package lists before installation
###############################################################################

set -euo pipefail
IFS=$'\n\t'

# Package lists
BUILD_DEPS=(
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
    x11-utils
)

RUNTIME_DEPS=(
    xvfb
    x11-utils
    libx11-6
    libxext6
    libxrandr2
    libxcursor1
    libxfixes3
    libxi6
    libwayland-client0
    libxkbcommon0
    libgl1
    libgles2
    libegl1
    libdbus-1-3
)

###############################################################################
# Usage Information
###############################################################################
usage() {
    cat << EOF
Usage: ${0##*/} [OPTIONS]

Install Ubuntu dependencies for Laya project.

OPTIONS:
    -h, --help              Show this help message
    -b, --build             Install build dependencies
    -r, --runtime           Install runtime dependencies
    -a, --all               Install all dependencies (build + runtime)

EXAMPLES:
    ${0##*/} --build
    ${0##*/} --runtime
    ${0##*/} --all

NOTES:
    - Requires sudo privileges
    - Updates apt package lists before installation
    - If no options specified, defaults to --all
EOF
    exit 0
}

###############################################################################
# Argument Parsing
###############################################################################
INSTALL_BUILD=false
INSTALL_RUNTIME=false

if [[ $# -eq 0 ]]; then
    INSTALL_BUILD=true
    INSTALL_RUNTIME=true
fi

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -b|--build)
            INSTALL_BUILD=true
            shift
            ;;
        -r|--runtime)
            INSTALL_RUNTIME=true
            shift
            ;;
        -a|--all)
            INSTALL_BUILD=true
            INSTALL_RUNTIME=true
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

echo "Updating package lists..."
sudo apt-get update

PACKAGES=()

if [[ "$INSTALL_BUILD" == true ]]; then
    echo "Adding build dependencies..."
    PACKAGES+=("${BUILD_DEPS[@]}")
fi

if [[ "$INSTALL_RUNTIME" == true ]]; then
    echo "Adding runtime dependencies..."
    PACKAGES+=("${RUNTIME_DEPS[@]}")
fi

if [[ ${#PACKAGES[@]} -eq 0 ]]; then
    echo "No packages to install"
    exit 0
fi

# Remove duplicates
PACKAGES=($(printf '%s\n' "${PACKAGES[@]}" | sort -u))

echo "Installing ${#PACKAGES[@]} packages..."
sudo apt-get install -y "${PACKAGES[@]}"

echo "✓ Dependencies installed successfully"
