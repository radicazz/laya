<#
.SYNOPSIS
    Run pre-commit hooks locally, optionally creating a disposable test commit.

.DESCRIPTION
    Uses the tools project's pre-commit installation to execute hooks locally.
    By default it runs `uv run --project tools pre-commit run` which will run hooks
    against staged files. Use `-All` to run against all files. Use `-TestCommit` to
    create an empty temporary commit to exercise commit-msg or commit-time hooks,
    then clean it up.

.PARAMETER All
    When specified, runs hooks across all files (equivalent to `pre-commit run --all-files`).

.PARAMETER TestCommit
    When specified, creates an empty test commit (no changes) to trigger commit-time hooks
    that only run during commit, then resets the repository state.

.EXAMPLE
    .\Run-Pre-Commit.ps1 -All

.EXAMPLE
    .\Run-Pre-Commit.ps1 -TestCommit
#>

[CmdletBinding()]
param(
    [switch]$All,
    [switch]$TestCommit
)

Set-StrictMode -Version Latest

## Resolve repository root and dot-source print helpers
$repoRoot = (Resolve-Path (Join-Path -Path $PSScriptRoot -ChildPath '..\..')).Path
$colorsPath = Join-Path -Path $repoRoot -ChildPath "scripts\PrintColor.ps1"
if (Test-Path $colorsPath) { . $colorsPath }

$toolsDir = Join-Path -Path $repoRoot -ChildPath 'tools'

if (-not (Test-Path $toolsDir)) {
    Write-ErrorMsg "tools directory not found: $toolsDir"
    exit 1
}

try {
    if ($All) {
        Write-Info "Running pre-commit for all files..."
        & uv run --project $toolsDir pre-commit run --all-files
    }
    elseif ($TestCommit) {
        Write-Info "Creating disposable test commit to trigger commit-time hooks..."
        # Create an empty commit (no-verify is intentionally NOT used)
        git commit --allow-empty -m "[test] pre-commit hook run" 2>$null
        $commitExit = $LASTEXITCODE
        if ($commitExit -ne 0) {
            Write-ErrorMsg "Failed to create test commit (git exit $commitExit). Ensure you have a git repo and no interactive prompts."
            exit 1
        }

        Write-Info "Running pre-commit hooks for this commit..."
        & uv run --project $toolsDir pre-commit run
        $runExit = $LASTEXITCODE

        Write-Info "Cleaning up test commit (reset to previous HEAD)..."
        git reset --hard HEAD~1 2>$null

        if ($runExit -ne 0) {
            Write-ErrorMsg "pre-commit run failed with exit code $runExit"
            exit $runExit
        }
    }
    else {
        Write-Info "Running pre-commit for staged files..."
        & uv run --project $toolsDir pre-commit run
    }
    Write-Success "pre-commit run completed successfully."
}
catch {
    Write-ErrorMsg "An error occurred: $_"
    exit 1
}
