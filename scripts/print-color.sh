#!/usr/bin/env bash
# shellcheck disable=SC2034
###############################################################################
# Script Name: print-color.sh
# Description : Shared color output helpers for Bash scripts in the Laya repo.
# Usage       : Source this file from other scripts
#               (e.g. `. "${SCRIPT_DIR}/../utils/print-color.sh"`).
# Notes       : Only emits color codes when stdout is a TTY. Provides
#               semantically named helper functions for consistent messaging.
# Dependencies: None (uses POSIX-compatible features only).
###############################################################################

set -euo pipefail
IFS=$'\n\t'

# Detect whether stdout is a TTY to decide if colors should be used.
if [[ -t 1 ]]; then
    RED="\e[31m"; GREEN="\e[32m"; YELLOW="\e[33m"; CYAN="\e[36m"; MAGENTA="\e[35m"; RESET="\e[0m"
else
    RED=""; GREEN=""; YELLOW=""; CYAN=""; MAGENTA=""; RESET=""
fi

info() { printf '%b[INFO] %s%b\n'   "$CYAN"   "$*" "$RESET"; }
success() { printf '%b[SUCCESS] %s%b\n' "$GREEN"  "$*" "$RESET"; }
warn() { printf '%b[WARN] %s%b\n'   "$YELLOW" "$*" "$RESET"; }
error() { printf '%b[ERROR] %s%b\n'  "$RED"    "$*" "$RESET"; }
debug() {
    if [[ ${LAYA_VERBOSE:-false} == "true" ]]; then
        printf '%b[DEBUG] %s%b\n' "$MAGENTA" "$*" "$RESET"
    fi
}
