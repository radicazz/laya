#!/usr/bin/env bash
#===============================================================================
# File: actions.sh
# Description: Local 'act' / Docker sanity check helper for the laya project.
# - Verifies 'act' is available on PATH or in ./bin/act
# - Verifies Docker is installed and the current user can access the daemon
# - Can attempt to install missing tools with -m/--install-missing
# - Safe by default: only performs changes when --install-missing is passed
# License: MIT
#===============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Usage/help
usage() {
    cat <<EOF
Usage: $(basename "$0") [options]

Options:
    -h, --help               Show this help message and exit
    -m, --install-missing    Attempt to install missing tools (requires sudo for system installs)

This is a lightweight developer helper to check for 'act' and Docker.
By default the script is read-only; pass --install-missing to attempt simple,
opinionated installs. Output uses icons: ✓ = ok, ✗ = missing, ⚠ = warning.
EOF
}

# basic arg parsing
INSTALL_MISSING=0
while [ $# -gt 0 ]; do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        -m|--install-missing)
            INSTALL_MISSING=1
            shift
            ;;
        *)
            echo "Unknown option: $1" >&2
            usage
            exit 2
            ;;
    esac
done

echo "Local GitHub Actions environment check"
echo "Repository: $REPO_ROOT"
echo

ACT_PATH=""
# Prefer system act, fallback to repository-local bin/act or .bin/act
if command -v act >/dev/null 2>&1; then
    ACT_PATH="$(command -v act)"
elif [ -x "$REPO_ROOT/bin/act" ]; then
    ACT_PATH="$REPO_ROOT/bin/act"
elif [ -x "$REPO_ROOT/.bin/act" ]; then
    ACT_PATH="$REPO_ROOT/.bin/act"
fi

if [ -n "$ACT_PATH" ]; then
    ACT_VER="$($ACT_PATH --version 2>/dev/null | head -n1 || true)"
    printf "✓ act: available -> %s %s\n" "$ACT_PATH" "$ACT_VER"
else
    printf "✗ act: not found (used for local GitHub Actions runs via 'act')\n"
    cat <<'ACT_HINT'
Recommended install options:
  - Official (Linux/macOS):
        curl -sSL https://raw.githubusercontent.com/nektos/act/master/install.sh | sudo bash
  - Or download a release binary and place it at ./bin/act (repo-local use).
  - On Windows, download the release and run via WSL.
ACT_HINT
    if [ $INSTALL_MISSING -eq 1 ]; then
        echo "Attempting to install 'act' (developer convenience)..."
        if command -v brew >/dev/null 2>&1; then
            brew install act || echo "brew install act failed"
        elif command -v curl >/dev/null 2>&1; then
            if command -v sudo >/dev/null 2>&1; then
                curl -sSL https://raw.githubusercontent.com/nektos/act/master/install.sh | sudo bash || echo "act install script failed"
            else
                curl -sSL https://raw.githubusercontent.com/nektos/act/master/install.sh | bash || echo "act install script failed"
            fi
        else
            echo "Cannot install 'act' automatically: missing 'curl' or 'brew'. Please install manually." >&2
        fi
        echo "Note: you may need to re-open your shell or place ./bin/act on PATH to use the binary."
    fi
fi

DOCKER_PATH=""
# Prefer system docker, fallback to repository-local bin/docker or .bin/docker
if command -v docker >/dev/null 2>&1; then
    DOCKER_PATH="$(command -v docker)"
elif [ -x "$REPO_ROOT/bin/docker" ]; then
    DOCKER_PATH="$REPO_ROOT/bin/docker"
elif [ -x "$REPO_ROOT/.bin/docker" ]; then
    DOCKER_PATH="$REPO_ROOT/.bin/docker"
fi

if [ -n "$DOCKER_PATH" ]; then
    DOCKER_VER="$($DOCKER_PATH --version 2>/dev/null | head -n1 || true)"
    printf "✓ docker: available -> %s %s\n" "$DOCKER_PATH" "$DOCKER_VER"
    if "$DOCKER_PATH" info >/dev/null 2>&1; then
        printf "✓ docker daemon: accessible\n"
    else
        printf "⚠ docker daemon: not accessible by current user (permission/connect issue)\n"
        cat <<'DOCK_HINT'
Possible fixes:
  - Add your user to the 'docker' group and re-login: sudo usermod -aG docker $USER
  - If using Docker Desktop (Windows/macOS), ensure the daemon is running and WSL 2 integration is enabled.
  - Start docker on Linux: sudo systemctl start docker
  - Check socket permissions: ls -l /var/run/docker.sock
DOCK_HINT
        if [ $INSTALL_MISSING -eq 1 ]; then
            echo "Attempting to add user to 'docker' group (requires sudo)..."
            if command -v sudo >/dev/null 2>&1; then
                sudo usermod -aG docker "$USER" || echo "usermod failed"
                echo "You must log out and back in for group changes to take effect."
            else
                echo "sudo not available; please run: sudo usermod -aG docker \$USER" >&2
            fi
        fi
    fi
else
    printf "✗ docker: not found (required to run 'act' jobs locally)\n"
    cat <<'DOCK_INSTALL'
Recommended install options:
  - Ubuntu/Debian: sudo apt-get update && sudo apt-get install -y docker.io
  - Official docs: https://docs.docker.com/get-docker/
  - Windows: Install Docker Desktop and enable WSL 2 integration
DOCK_INSTALL
    if [ $INSTALL_MISSING -eq 1 ]; then
        echo "Attempting to install Docker (developer convenience)..."
        if command -v apt-get >/dev/null 2>&1 && command -v sudo >/dev/null 2>&1; then
            sudo apt-get update && sudo apt-get install -y --no-install-recommends docker.io || echo "apt install docker.io failed"
        elif command -v brew >/dev/null 2>&1; then
            brew install --cask docker || echo "brew cask install docker failed"
            echo "Docker Desktop may require manual steps after installation."
        else
            echo "Automatic Docker install unsupported on this platform. Please follow: https://docs.docker.com/get-docker/" >&2
        fi
        echo "If you installed docker, ensure the daemon is running and consider adding your user to the 'docker' group."
    fi
fi

exit 0
