#!/bin/bash
set -euo pipefail

REPO="${GITHUB_REPOSITORY:-radicazz/laya}"

mkdir -p docs/ci-metrics

cat > docs/ci-metrics/index.md <<'EOF'
# CI/CD Metrics & Status

<div align="center">

[![Build](https://github.com/radicazz/laya/actions/workflows/build.yml/badge.svg)](https://github.com/radicazz/laya/actions/workflows/build.yml)
[![Tests](https://github.com/radicazz/laya/actions/workflows/test.yml/badge.svg)](https://github.com/radicazz/laya/actions/workflows/test.yml)
[![Benchmarks](https://github.com/radicazz/laya/actions/workflows/benchmark.yml/badge.svg)](https://github.com/radicazz/laya/actions/workflows/benchmark.yml)
[![Static Analysis](https://github.com/radicazz/laya/actions/workflows/static.yml/badge.svg)](https://github.com/radicazz/laya/actions/workflows/static.yml)

</div>

---

## 🏗️ Platform Support

| Platform | Compiler | Build Types | Status |
|----------|----------|-------------|--------|
| **Ubuntu** (ubuntu-latest) | GCC 13 | Debug + Release | ✅ Active |
| **Ubuntu** (ubuntu-latest) | Clang 18 | Debug + Release | ✅ Active |
| **Windows** (Server 2022) | MSVC 2022 | Debug + Release | ✅ Active |

## 🚀 Performance Benchmarks

> Benchmarks are run continuously on every build

| Platform | Compiler | Latest Results |
|----------|----------|----------------|
| **Linux** | GCC 13 | [View Results](https://github.com/radicazz/laya/actions/workflows/benchmark.yml) |
| **Linux** | Clang 18 | [View Results](https://github.com/radicazz/laya/actions/workflows/benchmark.yml) |
| **Windows** | MSVC 2022 | [View Results](https://github.com/radicazz/laya/actions/workflows/benchmark.yml) |

**Note**: Click "View Results" to see the latest benchmark artifacts from the most recent successful run.

## 📊 Workflow Links

<div align="center">

| Workflow | Status | Link |
|----------|--------|------|
| Build | ![Build](https://github.com/radicazz/laya/actions/workflows/build.yml/badge.svg) | [View Runs](https://github.com/radicazz/laya/actions/workflows/build.yml) |
| Tests | ![Tests](https://github.com/radicazz/laya/actions/workflows/test.yml/badge.svg) | [View Runs](https://github.com/radicazz/laya/actions/workflows/test.yml) |
| Benchmarks | ![Benchmarks](https://github.com/radicazz/laya/actions/workflows/benchmark.yml/badge.svg) | [View Runs](https://github.com/radicazz/laya/actions/workflows/benchmark.yml) |
| Static Analysis | ![Static](https://github.com/radicazz/laya/actions/workflows/static.yml/badge.svg) | [View Runs](https://github.com/radicazz/laya/actions/workflows/static.yml) |

</div>

EOF

CI_INFO_DIR="${CI_CLANG_INFO_DIR:-ci-meta/build-info}"

if compgen -G "${CI_INFO_DIR}"/*.md > /dev/null 2>&1; then
  {
    echo "---"
    echo
    echo "## 🔧 Latest Build Information"
    echo
  } >> docs/ci-metrics/index.md

  for info_file in "${CI_INFO_DIR}"/*.md; do
    cat "$info_file" >> docs/ci-metrics/index.md
    echo >> docs/ci-metrics/index.md
  done
fi
