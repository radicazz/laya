# Tools

Various tools to help with development, code formatting, testing and much more within the *laya* project. All of these tools are optional but highly recommended to ensure a smooth development experience.

> [!IMPORTANT]
> The contents of this folder are not meant to be executed directly. Rather visit [`scripts/`](../scripts/) to learn about the various helper scripts that will call these tools correctly.

## Setup

- These tools require Python 3.12 or newer.
- The [uv](https://uv.readthedocs.io/en/latest/) tool is used for dependency management.

The following scripts will install and configure the necessary tools for your environment. Call them from the project root directory.

```powershell
# Windows (PowerShell)
.\scripts\Verify-Tools.ps1 -InstallMissing
```

> [!NOTE]
> Unix (Linux/macOS) currently does not have a script to verify and install missing tools but it is planned.
