#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

###############################################################################
# Script Name: check-build.sh
# Description: Verifies essential build dependencies for the Laya project on Unix systems.
#              Checks for CMake, C/C++ compilers (with C++20 support), and build generators.
# Usage      : ./check-build.sh [OPTIONS]
# Options    : -h, --help  Display usage information
# Exit Codes : 0 = All dependencies satisfied
#              1 = Missing critical dependencies
# Platform   : Linux, macOS, WSL
# Notes      : Non-destructive - only checks and suggests installations.
#              Does not require sudo or modify the system.
#              Validates C++20 compiler support for modern C++ features.
###############################################################################

# Source color utilities
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COLOR_UTILS="${SCRIPT_DIR}/../utils/print-color.sh"
if [[ -f "$COLOR_UTILS" ]]; then
    # shellcheck source=../utils/print-color.sh
    . "$COLOR_UTILS"
else
    # Fallback if color utils not available
    info() { echo "[INFO] $*"; }
    success() { echo "[SUCCESS] $*"; }
    warn() { echo "[WARN] $*"; }
    error() { echo "[ERROR] $*"; }
fi

usage() {
    cat <<EOF
Usage: $(basename "$0") [OPTIONS]

Verifies essential build dependencies for the Laya project on Unix systems.

OPTIONS:
  -h, --help     Display this help message and exit

DESCRIPTION:
  This script checks for the presence and compatibility of build tools required
  to compile the Laya project:
    • CMake (minimum version 3.21)
    • C/C++ compiler with C++20 support (GCC 10+, Clang 10+, AppleClang 12+)
    • Build generator (Make or Ninja)

  The script is non-destructive and only reports findings. If dependencies are
  missing, it provides installation suggestions based on your package manager.

EXAMPLES:
  # Check build dependencies
  ./check-build.sh

  # Display help
  ./check-build.sh --help

EXIT CODES:
  0    All required dependencies are satisfied
  1    Missing critical dependencies

NOTES:
  • Requires at least one C/C++ compiler with C++20 support
  • CMake 3.21+ is required (Laya project minimum)
  • At least one build generator (Make/Ninja) must be available
  • CI/CD uses GCC 13 and Clang 18 for testing
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        *)
            error "Unknown argument: $1"
            usage
            exit 1
            ;;
    esac
done

# Track missing dependencies
MISSING_DEPS=()
FOUND_CMAKE=false
FOUND_COMPILER=false
FOUND_GENERATOR=false

# Detect OS for package manager suggestions
detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ -f /etc/os-release ]]; then
        . /etc/os-release
        echo "${ID:-linux}"
    else
        echo "linux"
    fi
}

OS_TYPE=$(detect_os)

info "=== Laya Build Dependency Check ==="
echo ""
info "Platform: $OSTYPE"
echo ""

# Check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Extract version from command output
get_version() {
    local cmd="$1"
    local version_flag="${2:---version}"
    if command_exists "$cmd"; then
        "$cmd" "$version_flag" 2>&1 | head -n 1 || true
    else
        echo "unknown"
    fi
}

# Check CMake version compatibility
check_cmake_version() {
    local cmake_path="$1"
    local version_output
    version_output=$("$cmake_path" --version 2>&1 | head -n 1) || true

    # Extract version number (e.g., "cmake version 3.28.1" -> "3.28.1")
    local version
    version=$(echo "$version_output" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n 1 || true)

    if [[ -z "$version" ]]; then
        echo "unknown|$version_output"
        return 1
    fi

    # Check if version >= 3.21 (Laya project requirement)
    local major minor
    major=$(echo "$version" | cut -d. -f1)
    minor=$(echo "$version" | cut -d. -f2)

    if [[ "$major" -gt 3 ]] || [[ "$major" -eq 3 && "$minor" -ge 21 ]]; then
        echo "ok|$version|$version_output"
        return 0
    else
        echo "too_old|$version|$version_output"
        return 1
    fi
}

# Check compiler C++20 support
check_cpp20_support() {
    local compiler="$1"
    local version_info="$2"

    # Handle empty or missing version info
    if [[ -z "$version_info" ]]; then
        echo "unknown|No version information available"
        return 1
    fi

    # Extract version numbers for different compilers
    if [[ "$compiler" == *"gcc"* ]] || [[ "$compiler" == *"g++"* ]]; then
        # GCC: version 10+ supports C++20
        local version
        version=$(echo "$version_info" | grep -oE 'gcc[^0-9]*[0-9]+\.[0-9]+' | grep -oE '[0-9]+\.[0-9]+' | head -n 1 || true)
        if [[ -z "$version" ]]; then
            version=$(echo "$version_info" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n 1 || true)
        fi

        if [[ -z "$version" ]]; then
            echo "unknown|Unable to parse GCC version from: $version_info"
            return 1
        fi

        local major
        major=$(echo "$version" | cut -d. -f1)

        if [[ -n "$major" ]] && [[ "$major" -ge 10 ]]; then
            echo "ok|GCC $version (C++20 supported)"
            return 0
        else
            echo "too_old|GCC $version (C++20 requires GCC 10+)"
            return 1
        fi

    elif [[ "$compiler" == *"clang"* ]]; then
        # Check for Apple Clang vs LLVM Clang
        if echo "$version_info" | grep -qi "apple"; then
            # Apple Clang: version 12+ supports C++20
            local version
            version=$(echo "$version_info" | grep -oE 'clang-[0-9]+' | grep -oE '[0-9]+' || true)
            if [[ -z "$version" ]]; then
                version=$(echo "$version_info" | grep -oE 'version [0-9]+\.[0-9]+' | grep -oE '[0-9]+\.[0-9]+' | head -n 1 || true)
            fi

            if [[ -z "$version" ]]; then
                echo "unknown|Unable to parse Apple Clang version from: $version_info"
                return 1
            fi

            local major
            major=$(echo "$version" | cut -d. -f1)

            if [[ -n "$major" ]] && [[ "$major" -ge 12 ]]; then
                echo "ok|Apple Clang $version (C++20 supported)"
                return 0
            else
                echo "too_old|Apple Clang $version (C++20 requires Apple Clang 12+)"
                return 1
            fi
        else
            # LLVM Clang: version 10+ supports C++20
            local version
            version=$(echo "$version_info" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n 1 || true)

            if [[ -z "$version" ]]; then
                version=$(echo "$version_info" | grep -oE '[0-9]+\.[0-9]+' | head -n 1 || true)
            fi

            if [[ -z "$version" ]]; then
                echo "unknown|Unable to parse Clang version from: $version_info"
                return 1
            fi

            local major
            major=$(echo "$version" | cut -d. -f1)

            if [[ -n "$major" ]] && [[ "$major" -ge 10 ]]; then
                echo "ok|Clang $version (C++20 supported)"
                return 0
            else
                echo "too_old|Clang $version (C++20 requires Clang 10+)"
                return 1
            fi
        fi
    fi

    echo "unknown|Unable to determine C++20 support"
    return 1
}

# ============================================================================
# Check CMake
# ============================================================================
info "Checking CMake..."
if command_exists cmake; then
    CMAKE_PATH=$(command -v cmake)
    CMAKE_CHECK=$(check_cmake_version "$CMAKE_PATH") || true
    IFS='|' read -r status version full_info <<< "$CMAKE_CHECK"

    if [[ "$status" == "ok" ]]; then
        success "CMake found: $CMAKE_PATH"
        echo "  Version: $full_info"
        FOUND_CMAKE=true
    elif [[ "$status" == "too_old" ]]; then
        warn "CMake found but version too old: $version"
        echo "  Path: $CMAKE_PATH"
        echo "  Required: CMake 3.21 or newer"
        MISSING_DEPS+=("cmake (version 3.21+)")
    else
        warn "CMake found but version check failed: $CMAKE_PATH"
        echo "  $full_info"
        MISSING_DEPS+=("cmake (version 3.21+)")
    fi
else
    error "CMake not found"
    MISSING_DEPS+=("cmake")
fi
echo ""

# ============================================================================
# Check C/C++ Compilers
# ============================================================================
info "Checking C/C++ compilers..."

# GCC Check
GCC_COMPILERS=("gcc-13" "gcc-12" "gcc-11" "gcc-10" "gcc")
GCC_FOUND=""
for gcc_cmd in "${GCC_COMPILERS[@]}"; do
    if command_exists "$gcc_cmd"; then
        GCC_FOUND="$gcc_cmd"
        break
    fi
done

if [[ -n "$GCC_FOUND" ]]; then
    GCC_PATH=$(command -v "$GCC_FOUND")
    GCC_VERSION=$(get_version "$GCC_FOUND")

    if [[ -z "$GCC_VERSION" ]] || [[ "$GCC_VERSION" == "unknown" ]]; then
        warn "GCC found but could not determine version: $GCC_PATH"
    else
        GCC_CPP20=$(check_cpp20_support "$GCC_FOUND" "$GCC_VERSION") || true
        IFS='|' read -r cpp20_status cpp20_info <<< "$GCC_CPP20"

        if [[ "$cpp20_status" == "ok" ]]; then
            success "GCC found: $GCC_PATH"
            echo "  $cpp20_info"
            FOUND_COMPILER=true
        else
            warn "GCC found but may not support C++20: $GCC_PATH"
            echo "  $cpp20_info"
        fi
    fi

    # Check for g++
    GXX_CMD="${GCC_FOUND/gcc/g++}"
    if command_exists "$GXX_CMD"; then
        GXX_PATH=$(command -v "$GXX_CMD")
        success "G++ found: $GXX_PATH"
    else
        warn "G++ not found (corresponding to $GCC_FOUND)"
    fi
else
    warn "GCC not found (optional, but recommended)"
fi
echo ""

# Clang Check
CLANG_COMPILERS=("clang-15" "clang-14" "clang-13" "clang-12" "clang-11" "clang-10" "clang")
CLANG_FOUND=""
for clang_cmd in "${CLANG_COMPILERS[@]}"; do
    if command_exists "$clang_cmd"; then
        CLANG_FOUND="$clang_cmd"
        break
    fi
done

if [[ -n "$CLANG_FOUND" ]]; then
    CLANG_PATH=$(command -v "$CLANG_FOUND")
    CLANG_VERSION=$(get_version "$CLANG_FOUND")

    if [[ -z "$CLANG_VERSION" ]] || [[ "$CLANG_VERSION" == "unknown" ]]; then
        warn "Clang found but could not determine version: $CLANG_PATH"
    else
        CLANG_CPP20=$(check_cpp20_support "$CLANG_FOUND" "$CLANG_VERSION") || true
        IFS='|' read -r cpp20_status cpp20_info <<< "$CLANG_CPP20"

        if [[ "$cpp20_status" == "ok" ]]; then
            success "Clang found: $CLANG_PATH"
            echo "  $cpp20_info"
            FOUND_COMPILER=true
        else
            warn "Clang found but may not support C++20: $CLANG_PATH"
            echo "  $cpp20_info"
        fi
    fi

    # Check for clang++
    CLANGXX_CMD="${CLANG_FOUND/clang/clang++}"
    if [[ "$CLANGXX_CMD" == "$CLANG_FOUND" ]]; then
        CLANGXX_CMD="clang++"
    fi
    if command_exists "$CLANGXX_CMD"; then
        CLANGXX_PATH=$(command -v "$CLANGXX_CMD")
        success "Clang++ found: $CLANGXX_PATH"
    else
        warn "Clang++ not found (corresponding to $CLANG_FOUND)"
    fi
else
    warn "Clang not found (optional alternative to GCC)"
fi
echo ""

# Check if we have at least one C++20 capable compiler
if [[ "$FOUND_COMPILER" == false ]]; then
    error "No C++20 capable compiler found"
    MISSING_DEPS+=("c++ compiler (GCC 10+, Clang 10+, or Apple Clang 12+)")
fi

# ============================================================================
# Check Build Generators
# ============================================================================
info "Checking build generators..."

# Ninja check
if command_exists ninja; then
    NINJA_PATH=$(command -v ninja)
    NINJA_VERSION=$(get_version ninja)
    success "Ninja found: $NINJA_PATH"
    echo "  $NINJA_VERSION"
    FOUND_GENERATOR=true
elif command_exists ninja-build; then
    NINJA_PATH=$(command -v ninja-build)
    NINJA_VERSION=$(get_version ninja-build)
    success "Ninja found: $NINJA_PATH"
    echo "  $NINJA_VERSION"
    FOUND_GENERATOR=true
else
    warn "Ninja not found (optional, but faster than Make)"
fi
echo ""

# Make check
if command_exists make; then
    MAKE_PATH=$(command -v make)
    MAKE_VERSION=$(get_version make)
    success "Make found: $MAKE_PATH"
    echo "  $MAKE_VERSION"
    FOUND_GENERATOR=true
else
    warn "Make not found"
fi
echo ""

if [[ "$FOUND_GENERATOR" == false ]]; then
    error "No build generator found (need Make or Ninja)"
    MISSING_DEPS+=("build generator (make or ninja)")
fi

# ============================================================================
# Installation Suggestions
# ============================================================================
if [[ ${#MISSING_DEPS[@]} -gt 0 ]]; then
    echo ""
    warn "=== Missing Dependencies Detected ==="
    for dep in "${MISSING_DEPS[@]}"; do
        echo "  ✗ $dep"
    done
    echo ""

    info "=== Installation Suggestions ==="
    echo ""

    case "$OS_TYPE" in
        macos)
            info "macOS (Homebrew):"
            echo "  # Install Homebrew if not already installed:"
            echo "  /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
            echo ""
            echo "  # Install dependencies:"
            if [[ "$FOUND_CMAKE" == false ]]; then
                echo "  brew install cmake"
            fi
            if [[ "$FOUND_COMPILER" == false ]]; then
                echo "  # Install Xcode Command Line Tools (includes Apple Clang):"
                echo "  xcode-select --install"
                echo ""
                echo "  # OR install LLVM Clang via Homebrew:"
                echo "  brew install llvm"
            fi
            if [[ "$FOUND_GENERATOR" == false ]]; then
                echo "  brew install ninja"
            fi
            ;;

        ubuntu|debian)
            info "Debian/Ubuntu (apt):"
            echo "  sudo apt-get update"
            apt_packages=()
            if [[ "$FOUND_CMAKE" == false ]]; then
                apt_packages+=("cmake")
            fi
            if [[ "$FOUND_COMPILER" == false ]]; then
                apt_packages+=("build-essential")
                echo "  # For GCC (minimum GCC 10+, CI uses GCC 13):"
            fi
            if [[ "$FOUND_GENERATOR" == false ]]; then
                apt_packages+=("ninja-build")
            fi
            if [[ ${#apt_packages[@]} -gt 0 ]]; then
                echo "  sudo apt-get install ${apt_packages[*]}"
            fi
            if [[ "$FOUND_COMPILER" == false ]]; then
                echo ""
                echo "  # For GCC 13 (matches CI environment):"
                echo "  sudo add-apt-repository ppa:ubuntu-toolchain-r/test"
                echo "  sudo apt-get update"
                echo "  sudo apt-get install gcc-13 g++-13"
                echo ""
                echo "  # OR for Clang (minimum Clang 10+, CI uses Clang 18):"
                echo "  sudo apt-get install clang"
                echo ""
                echo "  # For Clang 18 (matches CI environment):"
                echo "  wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -"
                echo "  sudo add-apt-repository 'deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-18 main'"
                echo "  sudo apt-get update"
                echo "  sudo apt-get install clang-18 clang++-18"
            fi
            ;;

        fedora|rhel|centos)
            info "Fedora/RHEL/CentOS (dnf/yum):"
            pkg_mgr="dnf"
            if ! command_exists dnf; then
                pkg_mgr="yum"
            fi
            packages=()
            if [[ "$FOUND_CMAKE" == false ]]; then
                packages+=("cmake")
            fi
            if [[ "$FOUND_COMPILER" == false ]]; then
                packages+=("gcc" "gcc-c++")
            fi
            if [[ "$FOUND_GENERATOR" == false ]]; then
                packages+=("ninja-build")
            fi
            if [[ ${#packages[@]} -gt 0 ]]; then
                echo "  sudo $pkg_mgr install ${packages[*]}"
            fi
            if [[ "$FOUND_COMPILER" == false ]]; then
                echo ""
                echo "  # OR for Clang (alternative):"
                echo "  sudo $pkg_mgr install clang"
            fi
            ;;

        arch|manjaro)
            info "Arch Linux (pacman):"
            packages=()
            if [[ "$FOUND_CMAKE" == false ]]; then
                packages+=("cmake")
            fi
            if [[ "$FOUND_COMPILER" == false ]]; then
                packages+=("base-devel")
            fi
            if [[ "$FOUND_GENERATOR" == false ]]; then
                packages+=("ninja")
            fi
            if [[ ${#packages[@]} -gt 0 ]]; then
                echo "  sudo pacman -S ${packages[*]}"
            fi
            ;;

        opensuse*)
            info "openSUSE (zypper):"
            packages=()
            if [[ "$FOUND_CMAKE" == false ]]; then
                packages+=("cmake")
            fi
            if [[ "$FOUND_COMPILER" == false ]]; then
                packages+=("gcc" "gcc-c++")
            fi
            if [[ "$FOUND_GENERATOR" == false ]]; then
                packages+=("ninja")
            fi
            if [[ ${#packages[@]} -gt 0 ]]; then
                echo "  sudo zypper install ${packages[*]}"
            fi
            ;;

        *)
            info "Generic Linux:"
            echo "  Use your distribution's package manager to install:"
            if [[ "$FOUND_CMAKE" == false ]]; then
                echo "    • cmake (version 3.21+)"
            fi
            if [[ "$FOUND_COMPILER" == false ]]; then
                echo "    • gcc/g++ (version 10+) or clang/clang++ (version 10+)"
                echo "    • CI/CD uses: GCC 13 or Clang 18"
            fi
            if [[ "$FOUND_GENERATOR" == false ]]; then
                echo "    • ninja or make"
            fi
            ;;
    esac
    echo ""
fi

# ============================================================================
# Summary
# ============================================================================
echo ""
info "=== Summary ==="
if [[ ${#MISSING_DEPS[@]} -eq 0 ]]; then
    success "✓ All required build dependencies are satisfied"
    success "✓ Ready to build Laya"
    echo ""
    info "Recommended build commands:"
    if [[ "$FOUND_GENERATOR" == true ]]; then
        if command_exists ninja || command_exists ninja-build; then
            echo "  cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release"
        else
            echo "  cmake -B build -DCMAKE_BUILD_TYPE=Release"
        fi
    else
        echo "  cmake -B build -DCMAKE_BUILD_TYPE=Release"
    fi
    echo "  cmake --build build --parallel"
    echo ""
    exit 0
else
    error "✗ Missing critical dependencies - build will fail"
    echo "  Run the suggested installation commands above to resolve"
    echo ""
    exit 1
fi
