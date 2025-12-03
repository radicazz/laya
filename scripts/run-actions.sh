#!/usr/bin/env bash
###############################################################################
# Script Name: run-actions.sh
# Description: Run GitHub Actions workflows locally using act
# Usage      : ./scripts/run-actions.sh [OPTIONS] [WORKFLOW]
# Examples   : ./scripts/run-actions.sh
#              ./scripts/run-actions.sh --workflow build
#              ./scripts/run-actions.sh --list
#              ./scripts/run-actions.sh --help
# Dependencies: act (https://github.com/nektos/act)
# Notes      : - Requires act to be installed and available on PATH
#              - Uses .actrc file in project root for configuration
#              - Workflows are located in .github/workflows/
###############################################################################

set -euo pipefail
IFS=$'\n\t'

# Get script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Source color utility
# shellcheck source=scripts/print-color.sh
. "${SCRIPT_DIR}/print-color.sh"

###############################################################################
# Usage Information
###############################################################################
usage() {
    cat << EOF
Usage: ${0##*/} [OPTIONS] [WORKFLOW]

Run GitHub Actions workflows locally using act.

OPTIONS:
    -h, --help              Show this help message
    -l, --list              List available workflows
    -w, --workflow NAME     Run specific workflow by name (e.g., build, test, docs)
    -j, --job JOB           Run specific job within workflow
    -e, --event EVENT       Trigger event type (default: push)
                            Options: push, pull_request, workflow_dispatch, etc.
    -v, --verbose           Enable verbose output
    -n, --dry-run           Dry run (show what would be executed)
    --container-architecture ARCH
                            Container architecture (auto, linux/amd64, linux/arm64)
    --pull                  Pull latest container images

WORKFLOW:
    Workflow file name without .yml extension (e.g., build, test, static, docs)
    If not specified, runs all push event workflows.

EXAMPLES:
    # List all available workflows
    ${0##*/} --list

    # Run build workflow
    ${0##*/} --workflow build

    # Run specific job in test workflow
    ${0##*/} --workflow test --job test-gcc-debug

    # Trigger pull request event
    ${0##*/} --workflow static --event pull_request

    # Dry run to see what would execute
    ${0##*/} --workflow build --dry-run

    # Run with verbose output
    ${0##*/} --workflow test --verbose

NOTES:
    - act must be installed: https://github.com/nektos/act
    - Container runtime (Docker/Podman) required
    - Some workflows may require GitHub secrets
    - Use .actrc in project root for persistent act configuration

EOF
}

###############################################################################
# Functions
###############################################################################

# Check if act is available
check_act_available() {
    if ! command -v act &> /dev/null; then
        error "act is not installed or not available on PATH"
        echo ""
        echo "Install act from: https://github.com/nektos/act"
        echo ""
        echo "Installation options:"
        echo "  - Arch Linux: sudo pacman -S act"
        echo "  - Homebrew:   brew install act"
        echo "  - Go:         go install github.com/nektos/act@latest"
        echo "  - Binary:     Download from GitHub releases"
        exit 1
    fi
}

# List available workflows
list_workflows() {
    local workflow_dir="$PROJECT_ROOT/.github/workflows"

    if [[ ! -d "$workflow_dir" ]]; then
        error "Workflows directory not found: $workflow_dir"
        exit 1
    fi

    info "Available workflows in .github/workflows/:"
    echo ""

    local count=0
    while IFS= read -r -d '' workflow; do
        local name
        name="$(basename "$workflow" .yml)"
        local description="N/A"
        if grep -q "^name:" "$workflow"; then
            description="$(grep -m 1 "^name:" "$workflow" | sed 's/name: *//')"
        fi
        printf "  %-20s - %s\n" "$name" "$description"
        count=$((count + 1))
    done < <(find "$workflow_dir" -maxdepth 1 -name "*.yml" -type f -print0 | sort -z)

    echo ""
    info "Total workflows found: $count"
}

# Validate workflow exists
validate_workflow() {
    local workflow_name="$1"
    local workflow_file="$PROJECT_ROOT/.github/workflows/${workflow_name}.yml"

    if [[ ! -f "$workflow_file" ]]; then
        error "Workflow not found: $workflow_file"
        echo ""
        info "Use --list to see available workflows"
        exit 1
    fi
}

###############################################################################
# Main Script
###############################################################################

# Parse command-line arguments
WORKFLOW=""
JOB=""
EVENT="push"
VERBOSE=false
DRY_RUN=false
LIST_WORKFLOWS=false
CONTAINER_ARCH=""
PULL_IMAGES=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        -l|--list)
            LIST_WORKFLOWS=true
            shift
            ;;
        -w|--workflow)
            WORKFLOW="$2"
            shift 2
            ;;
        -j|--job)
            JOB="$2"
            shift 2
            ;;
        -e|--event)
            EVENT="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -n|--dry-run)
            DRY_RUN=true
            shift
            ;;
        --container-architecture)
            CONTAINER_ARCH="$2"
            shift 2
            ;;
        --pull)
            PULL_IMAGES=true
            shift
            ;;
        *)
            # Assume it's a workflow name without flag
            if [[ -z "$WORKFLOW" ]]; then
                WORKFLOW="$1"
            else
                error "Unknown argument: $1"
                echo ""
                usage
                exit 1
            fi
            shift
            ;;
    esac
done

# Display header
printf '%b========================================%b\n' "$CYAN" "$RESET"
printf '%b  Laya - GitHub Actions Runner (act)%b\n' "$CYAN" "$RESET"
printf '%b========================================%b\n' "$CYAN" "$RESET"
echo ""

# Handle --list flag
if [[ "$LIST_WORKFLOWS" == "true" ]]; then
    list_workflows
    exit 0
fi

# Check if act is available (not needed when only listing workflows)
check_act_available

# Build act command
ACT_CMD="act"
ACT_ARGS=()

# Add event type
ACT_ARGS+=("$EVENT")

# Add workflow filter if specified
if [[ -n "$WORKFLOW" ]]; then
    validate_workflow "$WORKFLOW"
    ACT_ARGS+=("--workflows" ".github/workflows/${WORKFLOW}.yml")
    info "Running workflow: $WORKFLOW"
else
    info "Running all workflows for event: $EVENT"
fi

# Add job filter if specified
if [[ -n "$JOB" ]]; then
    ACT_ARGS+=("--job" "$JOB")
    info "Running job: $JOB"
fi

# Add verbose flag
if [[ "$VERBOSE" == "true" ]]; then
    ACT_ARGS+=("--verbose")
    info "Verbose mode enabled"
fi

# Add dry-run flag
if [[ "$DRY_RUN" == "true" ]]; then
    ACT_ARGS+=("--dryrun")
    info "Dry run mode enabled"
fi

# Add container architecture if specified
if [[ -n "$CONTAINER_ARCH" ]]; then
    ACT_ARGS+=("--container-architecture" "$CONTAINER_ARCH")
    info "Container architecture: $CONTAINER_ARCH"
fi

# Add pull flag if specified
if [[ "$PULL_IMAGES" == "true" ]]; then
    ACT_ARGS+=("--pull")
    info "Pull latest container images enabled"
fi

# Change to project root for act execution
cd "$PROJECT_ROOT"

# Display command
echo ""
printf '%b----------------------------------------%b\n' "$CYAN" "$RESET"
if [[ "$DRY_RUN" == "true" ]]; then
    info "Command (dry-run):"
else
    info "Executing:"
fi
echo "  $ACT_CMD ${ACT_ARGS[*]}"
printf '%b----------------------------------------%b\n' "$CYAN" "$RESET"
echo ""

# Execute act
if [[ "$DRY_RUN" == "true" ]]; then
    "$ACT_CMD" "${ACT_ARGS[@]}"
    echo ""
    success "Dry run completed"
else
    if "$ACT_CMD" "${ACT_ARGS[@]}"; then
        echo ""
        success "Workflow execution completed successfully"
    else
        echo ""
        error "Workflow execution failed"
        exit 1
    fi
fi
