#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

###############################################################################
# Script Name: check-actions.sh
# Description: Verifies local GitHub Actions testing environment for the Laya project.
#              Checks for 'act' and Docker availability for local workflow testing.
# Usage      : ./check-actions.sh [OPTIONS]
# Options    : -h, --help  Display usage information
# Exit Codes : 0 = All dependencies satisfied
#              1 = Missing critical dependencies
# Platform   : Linux, macOS, WSL
# Notes      : Non-destructive - only checks and suggests installations.
#              Does not require sudo or modify the system.
#              Used to verify local GitHub Actions testing with 'act'.
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

Verifies local GitHub Actions testing environment for the Laya project.

OPTIONS:
  -h, --help     Display this help message and exit

DESCRIPTION:
  This script checks for the presence of tools required to test GitHub Actions
  workflows locally using 'act':
    • act (nektos/act) - Local GitHub Actions runner
    • Docker - Container runtime required by act

  The script is non-destructive and only reports findings. If dependencies are
  missing, it provides installation suggestions based on your platform.

EXAMPLES:
  # Check GitHub Actions testing environment
  ./check-actions.sh

  # Display help
  ./check-actions.sh --help

EXIT CODES:
  0    All required dependencies are satisfied
  1    Missing critical dependencies

NOTES:
  • 'act' requires Docker to run GitHub Actions workflows locally
  • Docker must be running and accessible to the current user
  • Repository-local binaries in ./bin/ are also checked
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
FOUND_ACT=false
FOUND_DOCKER=false
DOCKER_ACCESSIBLE=false

# Detect repository root
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

info "=== GitHub Actions Local Testing Environment Check ==="
echo ""
info "Repository: $REPO_ROOT"
echo ""

# Check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# ============================================================================
# Check for 'act'
# ============================================================================
info "Checking for 'act' (nektos/act - Local GitHub Actions runner)..."

ACT_PATH=""
# Prefer system act, fallback to repository-local bin/act or .bin/act
if command_exists act; then
    ACT_PATH="$(command -v act)"
elif [[ -x "$REPO_ROOT/bin/act" ]]; then
    ACT_PATH="$REPO_ROOT/bin/act"
elif [[ -x "$REPO_ROOT/.bin/act" ]]; then
    ACT_PATH="$REPO_ROOT/.bin/act"
fi

if [[ -n "$ACT_PATH" ]]; then
    ACT_VERSION=$("$ACT_PATH" --version 2>/dev/null | head -n1 || echo "unknown")
    success "act found: $ACT_PATH"
    echo "  Version: $ACT_VERSION"
    FOUND_ACT=true
else
    error "act not found"
    MISSING_DEPS+=("act")
fi
echo ""

# ============================================================================
# Check for Docker
# ============================================================================
info "Checking for Docker..."

DOCKER_PATH=""
# Prefer system docker, fallback to repository-local bin/docker or .bin/docker
if command_exists docker; then
    DOCKER_PATH="$(command -v docker)"
elif [[ -x "$REPO_ROOT/bin/docker" ]]; then
    DOCKER_PATH="$REPO_ROOT/bin/docker"
elif [[ -x "$REPO_ROOT/.bin/docker" ]]; then
    DOCKER_PATH="$REPO_ROOT/.bin/docker"
fi

if [[ -n "$DOCKER_PATH" ]]; then
    DOCKER_VERSION=$("$DOCKER_PATH" --version 2>/dev/null | head -n1 || echo "unknown")
    success "Docker found: $DOCKER_PATH"
    echo "  Version: $DOCKER_VERSION"
    FOUND_DOCKER=true

    # Check if Docker daemon is accessible
    echo ""
    info "Checking Docker daemon accessibility..."
    if "$DOCKER_PATH" info >/dev/null 2>&1; then
        success "Docker daemon is accessible"
        DOCKER_ACCESSIBLE=true
    else
        warn "Docker daemon is not accessible by current user"
        echo "  The Docker daemon may not be running or you may lack permissions"
        MISSING_DEPS+=("docker daemon access")
    fi
else
    error "Docker not found"
    MISSING_DEPS+=("docker")
fi
echo ""

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

    # Detect OS for package manager suggestions
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        if [[ "$FOUND_ACT" == false ]]; then
            info "Install 'act' on macOS:"
            echo "  # Using Homebrew:"
            echo "  brew install act"
            echo ""
            echo "  # OR download binary manually:"
            echo "  curl -sSL https://raw.githubusercontent.com/nektos/act/master/install.sh | sudo bash"
            echo ""
        fi

        if [[ "$FOUND_DOCKER" == false ]]; then
            info "Install Docker on macOS:"
            echo "  # Download and install Docker Desktop:"
            echo "  https://docs.docker.com/desktop/install/mac-install/"
            echo ""
            echo "  # OR using Homebrew:"
            echo "  brew install --cask docker"
            echo "  # Note: Docker Desktop requires manual startup after installation"
            echo ""
        fi

        if [[ "$DOCKER_ACCESSIBLE" == false && "$FOUND_DOCKER" == true ]]; then
            info "Fix Docker daemon access on macOS:"
            echo "  # Ensure Docker Desktop is running"
            echo "  # Check: Docker icon should appear in menu bar"
            echo "  open -a Docker"
            echo ""
        fi

    elif [[ -f /etc/os-release ]]; then
        . /etc/os-release
        OS_ID="${ID:-linux}"

        case "$OS_ID" in
            ubuntu|debian)
                if [[ "$FOUND_ACT" == false ]]; then
                    info "Install 'act' on Ubuntu/Debian:"
                    echo "  # Using official install script:"
                    echo "  curl -sSL https://raw.githubusercontent.com/nektos/act/master/install.sh | sudo bash"
                    echo ""
                    echo "  # OR download and place in ./bin/ (repo-local):"
                    echo "  mkdir -p bin"
                    echo "  curl -sSL https://github.com/nektos/act/releases/latest/download/act_Linux_x86_64.tar.gz | tar xz -C bin"
                    echo "  chmod +x bin/act"
                    echo ""
                fi

                if [[ "$FOUND_DOCKER" == false ]]; then
                    info "Install Docker on Ubuntu/Debian:"
                    echo "  # Quick install (docker.io package):"
                    echo "  sudo apt-get update"
                    echo "  sudo apt-get install -y docker.io"
                    echo ""
                    echo "  # OR official Docker Engine:"
                    echo "  # Follow: https://docs.docker.com/engine/install/ubuntu/"
                    echo ""
                fi

                if [[ "$DOCKER_ACCESSIBLE" == false && "$FOUND_DOCKER" == true ]]; then
                    info "Fix Docker daemon access on Ubuntu/Debian:"
                    echo "  # Start Docker daemon:"
                    echo "  sudo systemctl start docker"
                    echo "  sudo systemctl enable docker"
                    echo ""
                    echo "  # Add current user to docker group (requires logout/login):"
                    echo "  sudo usermod -aG docker \$USER"
                    echo "  newgrp docker  # or logout and login"
                    echo ""
                    echo "  # Verify access:"
                    echo "  docker info"
                    echo ""
                fi
                ;;

            fedora|rhel|centos)
                if [[ "$FOUND_ACT" == false ]]; then
                    info "Install 'act' on Fedora/RHEL/CentOS:"
                    echo "  # Using official install script:"
                    echo "  curl -sSL https://raw.githubusercontent.com/nektos/act/master/install.sh | sudo bash"
                    echo ""
                fi

                if [[ "$FOUND_DOCKER" == false ]]; then
                    info "Install Docker on Fedora/RHEL/CentOS:"
                    echo "  # Fedora:"
                    echo "  sudo dnf install -y docker"
                    echo ""
                    echo "  # OR official Docker Engine:"
                    echo "  # Follow: https://docs.docker.com/engine/install/fedora/"
                    echo ""
                fi

                if [[ "$DOCKER_ACCESSIBLE" == false && "$FOUND_DOCKER" == true ]]; then
                    info "Fix Docker daemon access:"
                    echo "  sudo systemctl start docker"
                    echo "  sudo systemctl enable docker"
                    echo "  sudo usermod -aG docker \$USER"
                    echo "  newgrp docker"
                    echo ""
                fi
                ;;

            arch|manjaro)
                if [[ "$FOUND_ACT" == false ]]; then
                    info "Install 'act' on Arch Linux:"
                    echo "  # Using AUR:"
                    echo "  yay -S act  # or paru -S act"
                    echo ""
                    echo "  # OR official install script:"
                    echo "  curl -sSL https://raw.githubusercontent.com/nektos/act/master/install.sh | sudo bash"
                    echo ""
                fi

                if [[ "$FOUND_DOCKER" == false ]]; then
                    info "Install Docker on Arch Linux:"
                    echo "  sudo pacman -S docker"
                    echo ""
                fi

                if [[ "$DOCKER_ACCESSIBLE" == false && "$FOUND_DOCKER" == true ]]; then
                    info "Fix Docker daemon access:"
                    echo "  sudo systemctl start docker"
                    echo "  sudo systemctl enable docker"
                    echo "  sudo usermod -aG docker \$USER"
                    echo "  newgrp docker"
                    echo ""
                fi
                ;;

            *)
                if [[ "$FOUND_ACT" == false ]]; then
                    info "Install 'act' (Generic Linux):"
                    echo "  curl -sSL https://raw.githubusercontent.com/nektos/act/master/install.sh | sudo bash"
                    echo "  # OR visit: https://github.com/nektos/act"
                    echo ""
                fi

                if [[ "$FOUND_DOCKER" == false ]]; then
                    info "Install Docker (Generic Linux):"
                    echo "  # Follow official documentation:"
                    echo "  https://docs.docker.com/engine/install/"
                    echo ""
                fi
                ;;
        esac

    else
        # Unknown Unix-like system
        info "Install 'act' and Docker:"
        echo "  act: https://github.com/nektos/act"
        echo "  Docker: https://docs.docker.com/get-docker/"
        echo ""
    fi
fi

# ============================================================================
# Summary
# ============================================================================
echo ""
info "=== Summary ==="
if [[ ${#MISSING_DEPS[@]} -eq 0 ]]; then
    success "✓ All required dependencies are satisfied"
    success "✓ Ready to test GitHub Actions locally"
    echo ""
    info "Quick start commands:"
    echo "  # List available workflows:"
    echo "  act -l"
    echo ""
    echo "  # Run a specific job:"
    echo "  act -j <job-name>"
    echo ""
    echo "  # Run on push event:"
    echo "  act push"
    echo ""
    info "Documentation:"
    echo "  https://github.com/nektos/act"
    echo ""
    exit 0
else
    error "✗ Missing critical dependencies - cannot test GitHub Actions locally"
    echo "  Run the suggested installation commands above to resolve"
    echo ""
    exit 1
fi
