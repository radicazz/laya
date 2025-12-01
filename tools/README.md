# Tools

Various tools to help with development, code formatting, testing and much more within the *laya* project. All of these tools are optional but highly recommended to ensure a smooth development experience.

## Setup

These tools use [uv](https://uv.readthedocs.io/en/latest/) for Python version & dependency management.

## Formatting & Linting

Run the same checks CI enforces before sending a PR:

```bash
uv run --project tools --group dev pre-commit run --all-files
```

Other handy commands (run from the repo root unless noted):

- `uv run --project tools --group format black --check --diff <paths>` – verify Python formatting; drop `--check --diff` to apply fixes.
- `uv run --project tools --group format isort --profile black --check-only --diff <paths>` – ensure imports stay sorted exactly like CI.
- `uv run --project tools yamllint <files>` – lint YAML using `.yamllint`, matching the new pre-commit hook.

## Docs Server

To run the MkDocs documentation server locally for development:

```bash
# From the tools directory
cd tools
uv run mkdocs serve --config-file ../mkdocs.yml --watch ../docs
```

The documentation will be available at `http://127.0.0.1:8000/`.
