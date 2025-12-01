# Tools

Various tools to help with development, code formatting, testing and much more within the *laya* project. All of these tools are optional but highly recommended to ensure a smooth development experience.

## Requirements

- These tools require Python 3.12 or newer.
- The [uv](https://uv.readthedocs.io/en/latest/) tool is used for dependency management.

The following scripts will install and configure the necessary tools for your environment. Call them from the project root directory.

```powershell
# Windows (PowerShell)
.\scripts\Verify-Tools.ps1 -InstallMissing
```

> [!NOTE]
> Unix (Linux/macOS) currently does not have a script to verify and install missing tools but it is planned.

## Docs Server

To run the MkDocs documentation server locally for development:

```bash
# From the tools directory
cd tools
uv run mkdocs serve --config-file ../mkdocs.yml --watch ../docs
```

The documentation will be available at `http://127.0.0.1:8000/`.

