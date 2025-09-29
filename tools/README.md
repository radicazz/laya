# Tools

Various tools to help with development, formatting, testing and more within the project.

## Features

- Code formatting helpers for C++ (Python, CMake, YAML, Markdown, etc. to come)
- Code linting helpers for C++ (Python, CMake, YAML, Markdown, etc. to come)
- Pre-commit hooks to enforce code quality and consistency using `pre-commit`
- Easy setup and management of the tools environment using `uv` package manager

## Requirments

- Python 3.11 or higher
- [uv](https://uv.readthedocs.io/en/latest/) (Python package manager)

## Setup

The following scripts will install and configure the necessary tools for your environment. Call them from the project root directory.

```powershell
# Windows (PowerShell)
.\scripts\Verify-Tools.ps1 -InstallMissing
```

```bash
# Linux / macOS (Bash)
./scripts/verify-tools.sh --install-missing
```
