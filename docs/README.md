# Documentation (docs/)

This directory contains the project's documentation: guides, design notes,
roadmaps, implementation plans, and images used by the docs. The goal of this
README is to help contributors find, preview, and update documentation quickly
and consistently.

## What you'll find here

- `getting_started.md` — a short quick-start guide for new users.
- `shared_libraries.md` — details about building and deploying shared libraries
    (useful for developers working on build system or CI).
- `roadmap.md` — project roadmap and high level goals.
- `design/` — higher-level design documents and architecture discussions.
  - `implementation/` contains deeper implementation notes and technical
        specifications (CMake, testing, CI/CD, etc.).
- `assets/` — images and other media used by the documentation (logo, diagrams,
  etc.).

> [!NOTE]
> Nothing found in `design/` is final or set in stone. It's a dump of AI generated architectural ideas for brainstorming.

If you are looking for repository-wide contribution rules and coding style,
see the top-level `CONTRIBUTING.md` file.

## How to preview docs locally

You have a few lightweight options for previewing Markdown files while you
work:

- Use your editor's built-in Markdown preview (VS Code, JetBrains, etc.).
- Use a static site generator such as MkDocs or Docusaurus if you want a
    documentation site. For a quick MkDocs preview (optional):

```powershell
python -m pip install --user mkdocs mkdocs-material
mkdocs serve
```

This assumes you add or maintain a `mkdocs.yml` at the repo root. If you don't
use MkDocs, the editor preview is usually sufficient for writing and proofing
content.

## Writing and style guidelines

Keep documentation clear and concise. The following rules help keep the docs
consistent and review-ready:

- Use present-tense, imperative mood for instructions ("Run this command",
  "Open the file").
- Use short sections and subsections. Prefer small files focused on single
    topics rather than one very long page.
- Always include an example or command when describing how to run something.
- When adding images, put them in `docs/assets/` and reference them with a
    relative path. Provide an informative `alt` attribute.
- Use fenced code blocks with language tags for commands and snippets.
- Prefer prose-first: start with one or two sentences explaining the goal, then
    the steps.

Accessibility & formatting:

- Use headings (H2/H3) to structure pages. Avoid very deep nesting.
- Keep line length to ~80–100 characters for easier diffs.
- Run a spellchecker in your editor before opening a PR.

## Images and assets

- Store images in `docs/assets/` and keep filenames descriptive
    (e.g. `build-diagram.png`).
- Optimize images for web (PNG/JPG compression or SVG for diagrams).
- When you add a new image, reference it with a relative path, for example:

```markdown
![Build diagram](assets/build-diagram.svg)
```

## Contributing changes

1. Branch from `develop` for most changes. Create a small, focused branch for
    each docs change.
2. Open a pull request against `develop` with a short description of what you
    changed and why.
3. Link any related issue or discussion in the PR description.
4. Follow the PR checklist in `CONTRIBUTING.md` (formatting, spelling, tests,
   etc.).

If your change requires updating diagrams, CI configuration, or code snippets
in the docs, update those files and confirm the commands in the docs still run
locally (or note why they can't run in the PR description).

## Adding design or implementation docs

- Put high-level architecture and rationale documents under `design/`.
- Put step-by-step implementation notes, CI recipes, and CMake discussion under
    `design/implementation/`.
- When possible, include a short "Status" line near the top of long design
    docs to indicate whether the idea is "Draft", "In progress", or
    "Implemented".

## Common tasks and quick commands

- Preview a single Markdown file in VS Code: open the file and use "Open
    Preview" (Ctrl+Shift+V) or the preview button.
- Quick spellcheck suggestions: enable a Markdown linter or spellchecker
    extension in your editor.

## Useful scripts

- `scripts/unix-health-check` — verifies a local Ubuntu environment matches CI,
  checks runtime & build packages, and can optionally install missing packages
  with `--install-missing` (requires sudo). See `./scripts/unix-health-check --help` for details.

## Related files

- Root `CONTRIBUTING.md` — project contribution rules and PR checklist.
- `README.md` (repo root) — general project overview.
- `LICENSE.txt` — project license.

## Questions or help

If you're unsure where to put something or how to phrase a topic, open an
issue or a draft PR and tag a maintainer. Small iterative PRs are preferred —
they're easier to review and land.

Thanks for contributing to the Laya documentation!
