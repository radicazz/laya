# CI/CD Pipeline Documentation

This directory contains the GitHub Actions workflows for the Laya project's continuous integration and delivery pipeline.

## Pipeline Architecture

The CI pipeline is organized into 4 stages that run sequentially with dependencies:

```
Stage 1: Static Analysis (Gate)
           ↓
Stage 2: Build (Matrix: GCC/Clang/MSVC × Debug/Release)
           ↓
    ┌──────┴──────┐
    ↓             ↓
Stage 3A:      Stage 3B:
Tests          Benchmarks
(Debug)        (Release)
```

## Workflows

### 1. `static-analysis.yml` - Stage 1 (Gate)

**Triggers**: Push to master/develop, Pull Requests
**Purpose**: Fast feedback on code quality issues
**Duration**: ~3-5 minutes

**Jobs**:
- `format-check`: C++ (clang-format), Python (black, isort), file endings, YAML/JSON validation
- `lint-cpp`: clang-tidy analysis on all C++ code
- `static-analyzers`: cppcheck static analysis

**Artifacts**:
- `clang-tidy-reports` (30 days)
- `cppcheck-reports` (30 days)

**Failure Behavior**: Blocks all downstream workflows

### 2. `build.yml` - Stage 2

**Triggers**: After static-analysis succeeds
**Purpose**: Build across all compiler/platform combinations
**Duration**: ~8-12 minutes (parallel)

**Matrix**:
```yaml
Linux:
  - GCC 13 × Debug
  - GCC 13 × Release
  - Clang 18 × Debug
  - Clang 18 × Release

Windows:
  - MSVC 2022 × Debug
  - MSVC 2022 × Release
```

**Features**:
- AddressSanitizer enabled for all Debug builds
- FetchContent caching for SDL3 (~3min savings)
- Headless display (Xvfb) for Linux builds

**Artifacts** (90 days):
- `laya-linux-{compiler}-{build_type}`
- `laya-windows-msvc-{build_type}`

### 3A. `test.yml` - Stage 3A (Parallel with 3B)

**Triggers**: After build succeeds
**Purpose**: Run unit tests on Debug builds
**Duration**: ~3-5 minutes (parallel)

**Jobs**:
- `test-linux` (GCC Debug, Clang Debug)
- `test-windows` (MSVC Debug)

**Coverage**:
- GCC Debug: lcov coverage report + Codecov integration
- HTML coverage reports included

**Artifacts** (90 days):
- `test-results-linux-{compiler}-debug` (JUnit XML + console output)
- `test-results-windows-msvc-debug`
- `coverage-report-gcc-debug` (HTML + lcov)

### 3B. `benchmark.yml` - Stage 3B (Parallel with 3A)

**Triggers**: After build succeeds
**Purpose**: Run benchmarks on Release builds
**Duration**: ~5-8 minutes (parallel)

**Jobs**:
- `benchmark-linux` (GCC Release, Clang Release)
- `benchmark-windows` (MSVC Release)
- `aggregate-results`: Combines all benchmark results

**Output Formats**:
- JSON: Machine-readable for historical analysis
- TXT: Human-readable console output
- Metadata: Compiler, platform, commit info

**Artifacts** (365 days - extended):
- `benchmark-results-linux-{compiler}-release`
- `benchmark-results-windows-msvc-release`
- `benchmark-results-all-compilers` (aggregated)

## Workflow Dependencies

```yaml
Static Analysis → Build → Test (Debug only)
                       ↘ Benchmark (Release only)
```

## Caching Strategy

### FetchContent Cache (SDL3)
- **Key**: `${{ runner.os }}-${{ matrix.compiler }}-fetchcontent-${{ hashFiles('cmake/SetupSDL3.cmake') }}`
- **Path**: `build/_deps`
- **Savings**: ~2-3 minutes per build

### Python Packages
- **Action**: `actions/setup-python@v5` with `cache: 'pip'`
- **Savings**: ~30 seconds

## Artifact Retention

| Artifact Type | Retention | Reason |
|--------------|-----------|---------|
| Static Analysis Reports | 30 days | Short-term debugging |
| Build Artifacts | 90 days | Test/benchmark reuse |
| Test Results | 90 days | Historical test data |
| Coverage Reports | 90 days | Trend analysis |
| Benchmark Results | 365 days | Long-term performance tracking |

## Environment Variables

### All Workflows
- `ACT`: Set to 'false', used to skip artifact uploads when testing locally with [act](https://github.com/nektos/act)

### Linux Builds
- `DISPLAY`: Set to `:99` (Xvfb virtual display)
- `CC`, `CXX`: Compiler paths (gcc-13/g++-13 or clang-18/clang++-18)

## Manual Triggers

All workflows support `workflow_dispatch` for manual execution:

```bash
# Via GitHub CLI
gh workflow run static-analysis.yml
gh workflow run build.yml
gh workflow run test.yml
gh workflow run benchmark.yml
```

## Local Testing

### With act (nektos/act)
```bash
# Install act: https://github.com/nektos/act

# Test static analysis
act -j format-check

# Test build (requires large runner)
act -j build-linux --matrix compiler:gcc --matrix build_type:Debug
```

### Pre-commit Hooks
```bash
# Install pre-commit
pip install pre-commit

# Run all hooks
pre-commit run --all-files

# Run specific hook
pre-commit run clang-format --all-files
```

## Monitoring & Troubleshooting

### Common Issues

**Build slower than expected?**
- Check if FetchContent cache was invalidated
- First build downloads SDL3 (~3-5 min extra)

**Tests failing on Linux?**
- Verify Xvfb started successfully
- Check for AddressSanitizer reports in logs

**Benchmarks showing inconsistent results?**
- GitHub Actions runners have variable performance
- Compare trends across multiple runs
- Use JSON output for automated analysis

### Status Badges

Add to README.md:
```markdown
![Static Analysis](https://github.com/radicazz/laya/workflows/Static%20Analysis/badge.svg)
![Build](https://github.com/radicazz/laya/workflows/Build/badge.svg)
![Test](https://github.com/radicazz/laya/workflows/Test/badge.svg)
![Benchmark](https://github.com/radicazz/laya/workflows/Benchmark/badge.svg)
```

## Future Enhancements

Potential improvements:
- [ ] Performance regression detection in benchmarks
- [ ] Automatic PR comments with test/benchmark results
- [ ] ccache integration for faster rebuilds
- [ ] Container-based builds for reproducibility
- [ ] Matrix strategy for different SDL3 versions
- [ ] Coverage threshold enforcement
- [ ] Benchmark result visualization with GitHub Pages

## Contributing

When modifying workflows:
1. Test locally with `act` when possible
2. Use `workflow_dispatch` for testing on GitHub
3. Check artifact uploads are conditional on `${{ env.ACT != 'true' }}`
4. Update this README with any significant changes
5. Verify caching keys are still appropriate

## Questions?

For CI/CD issues, create an issue with the `ci/cd` label.
