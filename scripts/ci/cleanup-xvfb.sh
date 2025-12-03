#!/usr/bin/env bash
###############################################################################
# Script Name: cleanup-xvfb.sh
# Description: Stop Xvfb display server
# Usage      : ./scripts/ci/cleanup-xvfb.sh [PID]
# Examples   : ./scripts/ci/cleanup-xvfb.sh
#              ./scripts/ci/cleanup-xvfb.sh 12345
# Dependencies: None
# Notes      : - Reads XVFB_PID from environment if not provided
###############################################################################

set -euo pipefail
IFS=$'\n\t'

###############################################################################
# Usage Information
###############################################################################
usage() {
    cat << EOF
Usage: ${0##*/} [PID]

Stop Xvfb display server.

ARGUMENTS:
    PID                     Process ID of Xvfb (optional, uses XVFB_PID env var)

OPTIONS:
    -h, --help              Show this help message

EXAMPLES:
    ${0##*/}
    ${0##*/} 12345
EOF
    exit 0
}

###############################################################################
# Argument Parsing
###############################################################################
if [[ $# -eq 1 ]] && [[ "$1" == "-h" || "$1" == "--help" ]]; then
    usage
fi

# Use provided PID or fall back to environment variable
PID="${1:-${XVFB_PID:-}}"

###############################################################################
# Main
###############################################################################

if [[ -z "$PID" ]]; then
    echo "No Xvfb PID provided or found in environment"
    exit 0
fi

if sudo kill "$PID" 2>/dev/null; then
    echo "✓ Xvfb stopped (PID: $PID)"
else
    echo "Note: Xvfb process $PID already stopped or not found"
fi
