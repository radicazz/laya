# CI/CD Overview

The GitHub Actions in this directory implement a three-stage pipeline that mirrors the project's development process. Each stage produces the artifacts that downstream stages consume, so a failure short-circuits the rest of the pipeline.

| Stage | Workflow     | Trigger                                                            | Purpose                                                                               |
| ----- | ------------ | ------------------------------------------------------------------ | ------------------------------------------------------------------------------------- |
| 1     | `static.yml` | Push/PR to `main` or `develop`, manual dispatch                    | clang-format/black/isort + clang-tidy + cppcheck gate                                 |
| 2     | `build.yml`  | `workflow_run` after **Static Analysis** succeeds, manual dispatch | Matrix builds (Linux GCC/Clang Debug+Release, Windows MSVC Debug+Release) + artifacts |
| 3     | `test.yml`   | `workflow_run` after **Build** succeeds                            | Reuses Debug build artifacts to run unit tests + coverage + Codecov                   |
| —     | `docs.yml`   | Push/PR to `main`, manual dispatch                                 | Builds MkDocs site, publishes CI metrics page, deploys to GitHub Pages                |

### Flow at a Glance

```
Push/PR on main/develop
        |
        v
  Static Analysis (static.yml)
        |
        v
      Build (build.yml)
        |
        v
     Tests (test.yml)
        |
        v
      Docs Gate*  --(docs/** changed?)-->  Docs Deploy (docs.yml)

* Gate requires: branch==main, docs/ modified, and Static/Build/Test all succeeded.
```

## Stage Details

### Static Analysis (`static.yml`)

- Fast formatting/validation plus clang-tidy and cppcheck.
- All format steps now fail the job when violations are detected, so this workflow is the gate for the rest of the pipeline.

### Build (`build.yml`)

- Runs only when the Static Analysis workflow finishes successfully or via manual dispatch.
- Uses FetchContent caching for SDL3, installs GCC 13 or Clang 18, and enables AddressSanitizer only for Debug builds.
- Publishes Linux and Windows artifacts that already contain the `build/` tree root so downstream workflows can download them verbatim.

### Tests (`test.yml`)

- Triggered exclusively by the success of the Build workflow.
- Downloads the correct artifact from the triggering run via `run-id`, runs on Xvfb for Linux, publishes JUnit logs, and uploads GCC coverage data + Codecov.

### Documentation (`docs.yml`)

- Optional helper workflow that keeps the MkDocs site and the CI metrics page in sync with the latest build/test status badges.

## Shared Conventions

- All workflows set `ACT=false` so they can detect when they run under `nektos/act` and skip artifact uploads locally.
- Artifact retention: analysis (30 days), build/test artifacts (90 days), docs preview (7 days).
- `.github/scripts/generate-metrics.sh` generates the `docs/ci-metrics/index.md` page referenced by `docs.yml`.

## Note on Benchmarks

Benchmarks can be run locally using `./scripts/run-benchmarks.sh`. The benchmark workflow has been removed from CI/CD due to resource constraints, but the benchmarking infrastructure remains available for local performance testing.
