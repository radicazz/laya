<#!
.SYNOPSIS
    Runs pre-commit hooks using the repository's uv-managed tools environment.

.DESCRIPTION
    Executes pre-commit against staged files by default. Supports running against all
    files or within a disposable test commit to trigger commit-time hooks.

.PARAMETER All
    Runs hooks across all files (pre-commit run --all-files).

.PARAMETER TestCommit
    Creates an empty throwaway commit to exercise commit-time hooks, then resets the repo.

.PARAMETER Help
    Displays usage details.

.EXAMPLE
    .\Run-PreCommit.ps1

.EXAMPLE
    .\Run-PreCommit.ps1 -All
#>

[CmdletBinding()]
param(
    [switch]$All,
    [switch]$TestCommit,
    [switch]$Help
)

if ($Help) {
    Get-Help -Detailed $MyInvocation.MyCommand.Path
    return
}

Set-StrictMode -Version Latest

$repoRoot = (Resolve-Path (Join-Path -Path $PSScriptRoot -ChildPath '..\..')).Path
$colorsPath = Join-Path -Path $repoRoot -ChildPath 'scripts\utils\Print-Color.ps1'
if (Test-Path $colorsPath) { . $colorsPath }
if (-not (Get-Command -Name Write-Info -ErrorAction SilentlyContinue)) {
    function Write-Info { param([string]$Message) Write-Host "[INFO] $Message" }
}
if (-not (Get-Command -Name Write-Success -ErrorAction SilentlyContinue)) {
    function Write-Success { param([string]$Message) Write-Host "[SUCCESS] $Message" }
}
if (-not (Get-Command -Name Write-Warn -ErrorAction SilentlyContinue)) {
    function Write-Warn { param([string]$Message) Write-Warning $Message }
}
if (-not (Get-Command -Name Write-ErrorMsg -ErrorAction SilentlyContinue)) {
    function Write-ErrorMsg { param([string]$Message) Write-Error $Message }
}

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

    if ($LASTEXITCODE -ne 0) {
        Write-ErrorMsg "pre-commit run failed with exit code $LASTEXITCODE"
        exit $LASTEXITCODE
    }

    Write-Success "pre-commit run completed successfully."
}
catch {
    Write-ErrorMsg "An error occurred: $_"
    exit 1
}
