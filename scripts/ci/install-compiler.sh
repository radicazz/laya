#!/usr/bin/env bash
###############################################################################
# Script Name: install-compiler.sh
# Description: Install specific compiler versions on Ubuntu
# Usage      : ./scripts/ci/install-compiler.sh [OPTIONS]
# Examples   : ./scripts/ci/install-compiler.sh --gcc 13
#              ./scripts/ci/install-compiler.sh --clang 18
# Dependencies: apt-get, add-apt-repository
# Notes      : - Requires sudo privileges
#              - Sets CC and CXX environment variables for GitHub Actions
###############################################################################

set -euo pipefail
IFS=$'\n\t'

###############################################################################
# Usage Information
###############################################################################
usage() {
    cat << EOF
Usage: ${0##*/} [OPTIONS]

Install compiler on Ubuntu for CI/CD.

OPTIONS:
    -h, --help              Show this help message
    -g, --gcc VERSION       Install GCC version (e.g., 13)
    -c, --clang VERSION     Install Clang version (e.g., 18)
    --with-coverage         Install coverage tools (lcov for GCC)

EXAMPLES:
    ${0##*/} --gcc 13
    ${0##*/} --clang 18
    ${0##*/} --gcc 13 --with-coverage

NOTES:
    - Exports CC and CXX to GitHub Actions environment if available
    - GCC installed from ubuntu-toolchain-r/test PPA
    - Clang installed from apt.llvm.org repository
EOF
    exit 0
}

###############################################################################
# Argument Parsing
###############################################################################
COMPILER=""
VERSION=""
WITH_COVERAGE=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -g|--gcc)
            COMPILER="gcc"
            VERSION="$2"
            shift 2
            ;;
        -c|--clang)
            COMPILER="clang"
            VERSION="$2"
            shift 2
            ;;
        --with-coverage)
            WITH_COVERAGE=true
            shift
            ;;
        *)
            echo "Error: Unknown option: $1" >&2
            usage
            ;;
    esac
done

if [[ -z "$COMPILER" ]]; then
    echo "Error: No compiler specified" >&2
    usage
fi

###############################################################################
# Main
###############################################################################

if [[ "$COMPILER" == "gcc" ]]; then
    echo "Installing GCC ${VERSION}..."
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
    sudo apt-get update
    
    PACKAGES=(gcc-"${VERSION}" g++-"${VERSION}")
    
    if [[ "$WITH_COVERAGE" == true ]]; then
        PACKAGES+=(lcov)
    fi
    
    sudo apt-get install -y "${PACKAGES[@]}"
    
    CC="gcc-${VERSION}"
    CXX="g++-${VERSION}"
    
    echo "✓ GCC ${VERSION} installed"
    
elif [[ "$COMPILER" == "clang" ]]; then
    echo "Installing Clang ${VERSION}..."
    
    # Add LLVM repository key and source
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
    sudo add-apt-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-${VERSION} main" -y
    sudo apt-get update
    
    sudo apt-get install -y \
        clang-"${VERSION}" \
        clang++-"${VERSION}" \
        libc++-"${VERSION}"-dev \
        libc++abi-"${VERSION}"-dev
    
    CC="clang-${VERSION}"
    CXX="clang++-${VERSION}"
    
    echo "✓ Clang ${VERSION} installed"
fi

# Export to GitHub Actions environment if available
if [[ -n "${GITHUB_ENV:-}" ]]; then
    echo "CC=${CC}" >> "$GITHUB_ENV"
    echo "CXX=${CXX}" >> "$GITHUB_ENV"
    echo "Exported CC=${CC}, CXX=${CXX} to GitHub environment"
fi

# Also export for current shell
export CC
export CXX

echo "  CC=${CC}"
echo "  CXX=${CXX}"
