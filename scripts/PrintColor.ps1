<#
.SYNOPSIS
  Minimal cross-platform color printing helpers.

.DESCRIPTION
  Provides a small set of helper functions for scripts to print semantic
  messages (Info, Success, Warn, Error). They use ANSI VT sequences when the
  host supports virtual terminal sequences, otherwise fall back to
  Write-Host -ForegroundColor on Windows.
#>

param()

function Use-VT {
    try {
        return ($Host -and $Host.UI -and $Host.UI.SupportsVirtualTerminal) -eq $true
    }
    catch {
        return $false
    }
}

function Write-Info([string]$Message) {
    if (Use-VT) { Write-Host "`e[36m[INFO] $Message`e[0m" } else { Write-Host "[INFO] $Message" -ForegroundColor Cyan }
}

function Write-Success([string]$Message) {
    if (Use-VT) { Write-Host "`e[32m[SUCCESS] $Message`e[0m" } else { Write-Host "[SUCCESS] $Message" -ForegroundColor Green }
}

function Write-Warn([string]$Message) {
    if (Use-VT) { Write-Host "`e[33m[WARN] $Message`e[0m" } else { Write-Host "[WARN] $Message" -ForegroundColor Yellow }
}

function Write-ErrorMsg([string]$Message) {
    if (Use-VT) { Write-Host "`e[31m[ERROR] $Message`e[0m" } else { Write-Host "[ERROR] $Message" -ForegroundColor Red }
}

# This script is intended to be dot-sourced by scripts. Consumers should dot-source this file:
#   . "path\to\PrintColor.ps1"
