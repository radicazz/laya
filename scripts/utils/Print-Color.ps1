<#
.SYNOPSIS
    Minimal cross-platform color printing helpers.

.DESCRIPTION
    Provides helper functions to print semantic messages (Info, Success, Warn, Error, Debug)
    with ANSI virtual terminal sequences when supported. Scripts can dot-source this module
    to obtain consistent, portable colored output across platforms.

.EXAMPLE
    . "${PSScriptRoot}\Print-Color.ps1"
    Write-Info "Checking dependencies..."

.NOTES
    This script is intended to be dot-sourced. It has no public parameters and produces
    no output when imported.
#>

param()

Set-StrictMode -Version Latest

function Use-VT {
    try {
        return ($Host -and $Host.UI -and $Host.UI.SupportsVirtualTerminal) -eq $true
    }
    catch {
        return $false
    }
}

function Write-Info {
    param([Parameter(Mandatory)][string]$Message)
    if (Use-VT) { Write-Host "`e[36m[INFO] $Message`e[0m" } else { Write-Host "[INFO] $Message" -ForegroundColor Cyan }
}

function Write-Success {
    param([Parameter(Mandatory)][string]$Message)
    if (Use-VT) { Write-Host "`e[32m[SUCCESS] $Message`e[0m" } else { Write-Host "[SUCCESS] $Message" -ForegroundColor Green }
}

function Write-Warn {
    param([Parameter(Mandatory)][string]$Message)
    if (Use-VT) { Write-Host "`e[33m[WARN] $Message`e[0m" } else { Write-Host "[WARN] $Message" -ForegroundColor Yellow }
}

function Write-ErrorMsg {
    param([Parameter(Mandatory)][string]$Message)
    if (Use-VT) { Write-Host "`e[31m[ERROR] $Message`e[0m" } else { Write-Host "[ERROR] $Message" -ForegroundColor Red }
}

function Write-DebugMsg {
    param([Parameter(Mandatory)][string]$Message)
    if ($env:LAYA_VERBOSE -and $env:LAYA_VERBOSE.ToLower() -eq 'true') {
        if (Use-VT) { Write-Host "`e[35m[DEBUG] $Message`e[0m" } else { Write-Host "[DEBUG] $Message" -ForegroundColor Magenta }
    }
}

# Consumers should dot-source this file from their scripts, e.g.:
#   . "${RepoRoot}\scripts\utils\Print-Color.ps1"
