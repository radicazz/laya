# Tools

Various tools to help with development, code formatting, testing and much more within the *laya* project. All of these tools are optional but highly recommended to ensure a smooth development experience.

## Setup

These tools use [uv](https://uv.readthedocs.io/en/latest/) for Python version & dependency management.

## Docs Server

To run the MkDocs documentation server locally for development:

```bash
# From the tools directory
cd tools
uv run mkdocs serve --config-file ../mkdocs.yml --watch ../docs
```

The documentation will be available at `http://127.0.0.1:8000/`.
