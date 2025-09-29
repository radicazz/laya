# Scripting Style Guide

This document defines conventions for Bash (`.sh`) and PowerShell (`.ps1`) scripts used in this project. The goal is to enforce **clarity, consistency, and portability** across platforms.

---

## General Rules

- All scripts must include **neat, consistent, detailed, and legible internal documentation**.
- Words in script names and parameters are separated with **hyphens (`-`)**.
- Scripts intended for both platforms must share the same logical name, e.g.:
  - `verify-tools.sh` (Linux/macOS/WSL)
  - `Verify-Tools.ps1` (Windows)
- Each script must provide **help/usage information**:
  - Bash → `-h` / `--help`
  - PowerShell → `Get-Help`

- If a script supports an "install missing" mode (for automatically installing required tools/dependencies), it MUST prompt the user *before performing any installation* with a clear, single confirmation message that:
  - Summarizes what will be installed (package/tool name and source), for example:

    "Install Missing Enabled - This script will automatically install\n    - uv (from PyPI)\n    - docker (via system package manager)\n    Do you wish to continue? (y/n)"

  - Lists the package(s) and their expected sources (e.g. PyPI, apt, brew, pipx, GitHub releases). The prompt must require an explicit positive confirmation (e.g. `y` or `Y`) before proceeding. If the user declines, the script must exit without making changes.

  - Include a TODO note in the script header if alternative, non-automatic installation strategies are planned (requirements.txt, manual steps, or other package managers).

---

## Bash Scripts (`.sh`)

- Shebang:

  ```bash
  #!/usr/bin/env bash
  set -euo pipefail
  IFS=$'\n\t'
  ```

- File names: **lowercase**, words separated with `-`.
  - Example: `verify-tools.sh`, `build-release.sh`.
- Parameters: **short** and **long** flags.
  - Example: `-h`, `--help`, `--install-missing`.
- Required:
  - Implement `usage()` function with detailed examples.
  - Provide `-h`/`--help` flags that call `usage()`.

### Install-missing confirmation (Bash)

- When implementing `--install-missing` the script must prompt before doing any installs. Example pattern:

```bash
echo "Install Missing Enabled - This script will automatically install"
echo " - uv (from PyPI)"
echo " - docker (via apt/brew)"
read -r -p "Do you wish to continue? (y/N) " response
case "$response" in
  [yY]) echo "Proceeding..." ;;
  *) echo "Cancelled by user."; exit 0 ;;
esac
```

---

## PowerShell Scripts (`.ps1`)

- File names: **PascalCase**, words separated with `-`.
  - Example: `Verify-Tools.ps1`, `Build-Release.ps1`.
- Functions: follow PowerShell **Verb-Noun** convention.
  - Example: `Verify-Tools`, `Build-Release`.
- Parameters: **PascalCase** with `-` prefix.
  - Example: `-Help`, `-InstallMissing`.
- Required:
  - Include **comment-based help** (`.SYNOPSIS`, `.DESCRIPTION`, `.PARAMETER`, `.EXAMPLE`).
  - Must work with `Get-Help ScriptName`.
- Help switch: provide `-Help` (and optionally `-?`) to display usage inline.

### Install-missing confirmation (PowerShell)

- When implementing `-InstallMissing` the script must prompt before doing any installs. Use `Read-Host` and require an explicit `Y`/`y` response. Example pattern:

```powershell
Write-Warning "Install Missing Enabled - This script will automatically install:`n - uv (from PyPI)`n - docker (via system package manager)"
$response = Read-Host "Do you wish to continue? (y/N)"
if ($response -ne 'y' -and $response -ne 'Y') { Write-Host "Cancelled by user."; exit 0 }
```

Include the same list-of-packages-and-sources policy as Bash (show where each package will come from).

### Portable color printing (both platforms)

Scripts should use a small set of well-named helpers for printing colored and prefixed messages. Use color only when output is a TTY and provide a non-colored fallback when not.

Recommended semantic kinds and colors:

- success: green
- info: cyan
- warn: yellow
- error: red
- debug: magenta (optional, gated behind a verbose/debug flag)

Example Bash helpers (ANSI fallback):

```bash
# Detect TTY and support colors
if [[ -t 1 ]]; then
  RED="\e[31m"; GREEN="\e[32m"; YELLOW="\e[33m"; CYAN="\e[36m"; MAGENTA="\e[35m"; RESET="\e[0m"
else
  RED=""; GREEN=""; YELLOW=""; CYAN=""; MAGENTA=""; RESET=""
fi

info() { printf "%b[INFO] %s%b\n" "$CYAN" "$*" "$RESET"; }
success() { printf "%b[SUCCESS] %s%b\n" "$GREEN" "$*" "$RESET"; }
warn() { printf "%b[WARN] %s%b\n" "$YELLOW" "$*" "$RESET"; }
error() { printf "%b[ERROR] %s%b\n" "$RED" "$*" "$RESET"; }
```

Example PowerShell helpers (cross-platform):

```powershell
function Write-Info([string]$Message) { if ($Host.UI.SupportsVirtualTerminal) { Write-Host "`e[36m[INFO] $Message`e[0m" } else { Write-Host "[INFO] $Message" -ForegroundColor Cyan } }
function Write-Success([string]$Message) { if ($Host.UI.SupportsVirtualTerminal) { Write-Host "`e[32m[SUCCESS] $Message`e[0m" } else { Write-Host "[SUCCESS] $Message" -ForegroundColor Green } }
function Write-Warn([string]$Message) { if ($Host.UI.SupportsVirtualTerminal) { Write-Host "`e[33m[WARN] $Message`e[0m" } else { Write-Host "[WARN] $Message" -ForegroundColor Yellow } }
function Write-ErrorMsg([string]$Message) { if ($Host.UI.SupportsVirtualTerminal) { Write-Host "`e[31m[ERROR] $Message`e[0m" } else { Write-Host "[ERROR] $Message" -ForegroundColor Red } }
```

Notes:

- Prefer these helper names rather than calling raw escape sequences or `Write-Host`/`printf` all over the script. Keep coloring logic in one place.
- Avoid bright/bold variants and heavy emoji usage; stick to clear textual prefixes `[INFO]`, `[WARN]`, `[ERROR]`, `[SUCCESS]` so logs remain parseable.
- When writing CI scripts, detect non-TTY output and disable colors to keep logs clean.
- Document these helpers at the top of each script (or source a shared small `lib/colors.sh` / `lib/Colors.ps1` helper in the repo).

---

## Platform Scope

- **Bash scripts** → Linux, macOS, WSL environments.
- **PowerShell scripts** → Windows environments.
- CI/CD should call the platform-appropriate script (`.sh` on Linux/macOS, `.ps1` on Windows).

---

## Examples

**Bash:**

```bash
./verify-tools.sh --install-missing
./build-release.sh -h
```

**PowerShell:**

```powershell
.\Verify-Tools.ps1 -InstallMissing
Get-Help .\Build-Release.ps1 -Detailed
```
