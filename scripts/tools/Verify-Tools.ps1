<#
.SYNOPSIS
    Verifies and sets up development tools using uv and pre-commit.

.DESCRIPTION
    This script checks for the 'uv' Python package manager. If not found and Install-Missing is specified,
    it attempts to install uv automatically. It then ensures the tools directory has the necessary
    virtual environment and dependencies synced. Finally, it installs pre-commit hooks using the tools environment.

.NOTES
    This script assumes it is run from the root directory of the Laya project.
    TODO: Support alternative methods like requirements.txt for users without uv.
#>

[CmdletBinding()]
param(
    [switch]$InstallMissing
)

# Enable strict mode for better error handling
Set-StrictMode -Version Latest

# Calculate repository root and dot-source color helpers (optional; non-fatal)
$repoRoot = (Resolve-Path (Join-Path -Path $PSScriptRoot -ChildPath '..\..')).Path
$colorsPath = Join-Path -Path $repoRoot -ChildPath "scripts\PrintColor.ps1"
if (Test-Path $colorsPath) { . $colorsPath } else { Write-Verbose "Colors helper not found at $colorsPath; continuing without colored output." }

# If Install-Missing is specified, warn about potential installations
if ($InstallMissing) {
    Write-Warn "This script may install the 'uv' dependency from PyPI using pip when required."
    $confirm = Read-Host "Do you want to continue? (y/n)"
    if ($confirm -ne 'y') {
        Write-Info "Operation cancelled by user."
        exit 0
    }
}

# Check for uv installation
Write-Verbose "Checking for uv installation..."
try {
    $uvVersion = & uv --version 2>$null
    if ($LASTEXITCODE -ne 0) {
        throw "uv command failed"
    }
    Write-Success "uv found: $uvVersion"
}
catch {
    if ($InstallMissing) {
        Write-Warn "uv not found. Attempting to install uv..."
        Write-Verbose "Running: pip install uv"
        & pip install uv
        if ($LASTEXITCODE -ne 0) {
            Write-ErrorMsg "Failed to install uv using pip. Please install uv manually."
            exit 1
        }
        # Verify installation
        try {
            $uvVersion = & uv --version 2>$null
            if ($LASTEXITCODE -ne 0) {
                throw "Verification failed"
            }
            Write-Success "uv installed successfully: $uvVersion"
        }
        catch {
            Write-Error "uv installation verification failed."
            exit 1
        }
    }
    else {
        Write-ErrorMsg "uv is not installed or not accessible."
        Write-Info "To install uv, run: pip install uv"
        Write-Info "Or follow the instructions at: https://github.com/astral-sh/uv"
        Write-Info "Alternatively, use the -InstallMissing parameter to install automatically."
        exit 1
    }
}

# Define absolute tools directory (repo-root based)
$toolsDir = Join-Path -Path $repoRoot -ChildPath 'tools'

# Check if tools directory exists
if (-not (Test-Path $toolsDir)) {
    Write-ErrorMsg "Tools directory '$toolsDir' does not exist."
    exit 1
}

# Navigate to tools directory and ensure fresh sync
Push-Location $toolsDir
try {
    # Remove existing virtual environment and lock file for a clean sync
    if (Test-Path ".venv") {
        Write-Verbose "Removing existing .venv directory..."
        Remove-Item ".venv" -Recurse -Force
    }
    if (Test-Path "uv.lock") {
        Write-Verbose "Removing existing uv.lock file..."
        Remove-Item "uv.lock" -Force
    }

    Write-Info "Syncing dependencies in tools directory..."
    Write-Verbose "Running: uv sync --all-groups"
    & uv sync --all-groups
    if ($LASTEXITCODE -ne 0) {
        Write-ErrorMsg "uv sync failed with exit code $LASTEXITCODE"
        exit 1
    }
}
finally {
    Pop-Location
}

# Install pre-commit hooks from project root
Write-Info "Installing pre-commit hooks..."
Write-Verbose "Running: uv run --project $toolsDir pre-commit install"
& uv run --project $toolsDir pre-commit install
if ($LASTEXITCODE -ne 0) {
    Write-ErrorMsg "pre-commit install failed with exit code $LASTEXITCODE"
    exit 1
}

Write-Success "Tooling verification and setup completed successfully."
