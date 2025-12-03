#!/usr/bin/env bash
###############################################################################
# Script Name: setup-xvfb.sh
# Description: Setup and verify headless X11 display for GUI testing
# Usage      : ./scripts/ci/setup-xvfb.sh [OPTIONS]
# Examples   : ./scripts/ci/setup-xvfb.sh
#              ./scripts/ci/setup-xvfb.sh --display :1
#              ./scripts/ci/setup-xvfb.sh --resolution 1280x720x24
# Dependencies: Xvfb, xdpyinfo
# Notes      : - Sets XVFB_PID and DISPLAY environment variables
#              - Designed for CI/CD environments
###############################################################################

set -euo pipefail
IFS=$'\n\t'

# Default values
DISPLAY_NUM=":99"
RESOLUTION="1920x1080x24"
WAIT_TIME=3

###############################################################################
# Usage Information
###############################################################################
usage() {
    cat << EOF
Usage: ${0##*/} [OPTIONS]

Setup headless X11 display server for GUI testing.

OPTIONS:
    -h, --help              Show this help message
    -d, --display NUM       Display number (default: :99)
    -r, --resolution RES    Screen resolution (default: 1920x1080x24)
    -w, --wait SECONDS      Wait time after start (default: 3)

EXAMPLES:
    ${0##*/}
    ${0##*/} --display :1 --resolution 1280x720x24

NOTES:
    - Exports XVFB_PID and DISPLAY to GitHub Actions environment
    - Use scripts/ci/cleanup-xvfb.sh to stop the display server
EOF
    exit 0
}

###############################################################################
# Argument Parsing
###############################################################################
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -d|--display)
            DISPLAY_NUM="$2"
            shift 2
            ;;
        -r|--resolution)
            RESOLUTION="$2"
            shift 2
            ;;
        -w|--wait)
            WAIT_TIME="$2"
            shift 2
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

echo "Starting Xvfb on display ${DISPLAY_NUM} with resolution ${RESOLUTION}..."

# Start Xvfb in background
sudo Xvfb "${DISPLAY_NUM}" -screen 0 "${RESOLUTION}" -ac +extension GLX +render -noreset > /dev/null 2>&1 &
XVFB_PID=$!

# Export to GitHub Actions environment if available
if [[ -n "${GITHUB_ENV:-}" ]]; then
    echo "XVFB_PID=${XVFB_PID}" >> "$GITHUB_ENV"
    echo "DISPLAY=${DISPLAY_NUM}" >> "$GITHUB_ENV"
fi

# Also export for current shell
export DISPLAY="${DISPLAY_NUM}"
export XVFB_PID

# Wait for Xvfb to start
sleep "${WAIT_TIME}"

# Verify display is working
if ! xdpyinfo -display "${DISPLAY_NUM}" >/dev/null 2>&1; then
    echo "ERROR: Xvfb failed to start on display ${DISPLAY_NUM}" >&2
    exit 1
fi

echo "✓ Virtual display ${DISPLAY_NUM} ready (${RESOLUTION})"
echo "  PID: ${XVFB_PID}"
