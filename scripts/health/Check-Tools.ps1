<#!
.SYNOPSIS
    Verifies required tooling artifacts for the Laya repository.

.DESCRIPTION
    Ensures the "uv" package manager is available on PATH and checks that the tools
    project has been synchronised (presence of .venv/, pyproject.toml, uv.lock).
    Emits actionable guidance when prerequisites are missing.

.PARAMETER ToolsDirectory
    Optional path to the tools project. Defaults to the repository's "tools" folder.

.PARAMETER Help
    Displays this help text.

.EXAMPLE
    .\Check-Tools.ps1

.EXAMPLE
    .\Check-Tools.ps1 -ToolsDirectory ..\custom-tools

.NOTES
    TODO: Document alternative dependency managers for environments without uv.
#>

[CmdletBinding()]
param(
    [Parameter(Position = 0)][string]$ToolsDirectory,
    [switch]$Help
)

if ($Help) {
    Get-Help -Detailed $MyInvocation.MyCommand.Path
    return
}

Set-StrictMode -Version Latest

$repoRoot = (Resolve-Path (Join-Path -Path $PSScriptRoot -ChildPath '..\..')).Path
$defaultToolsDir = Join-Path -Path $repoRoot -ChildPath 'tools'
if (-not $ToolsDirectory) { $ToolsDirectory = $defaultToolsDir }

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

if (-not (Test-Path $ToolsDirectory)) {
    Write-ErrorMsg "Tools directory not found: $ToolsDirectory"
    Write-Info "Ensure you are running this script from within the repository checkout."
    exit 1
}

# region: uv availability
$uvCommand = Get-Command -Name 'uv' -ErrorAction SilentlyContinue
if (-not $uvCommand) {
    Write-ErrorMsg "The 'uv' CLI was not found on PATH."
    Write-Info "Install instructions:"
    Write-Info " - pipx install uv"
    Write-Info " - pip install uv"
    Write-Info " - Visit https://github.com/astral-sh/uv for platform-specific packages"
    exit 1
}
Write-Success "uv located at: $($uvCommand.Source)"
# endregion

$resolvedTools = (Resolve-Path -Path $ToolsDirectory).Path
$venvPath = Join-Path -Path $resolvedTools -ChildPath '.venv'
$pyprojectPath = Join-Path -Path $resolvedTools -ChildPath 'pyproject.toml'
$uvLockPath = Join-Path -Path $resolvedTools -ChildPath 'uv.lock'

$missingArtifacts = @()
if (-not (Test-Path $venvPath)) { $missingArtifacts += '.venv directory' }
if (-not (Test-Path $pyprojectPath)) { $missingArtifacts += 'pyproject.toml' }
if (-not (Test-Path $uvLockPath)) { $missingArtifacts += 'uv.lock' }

if ($missingArtifacts.Count -eq 0) {
    Write-Success "Tools directory is properly synchronised."
    exit 0
}

Write-Warn "The following tooling artifacts are missing:"
foreach ($artifact in $missingArtifacts) {
    Write-Warn " - $artifact"
}

Write-Info "Execute 'uv sync --all-extras' within $resolvedTools to regenerate them."
exit 1
