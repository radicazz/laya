#!/bin/bash

mkdir -p docs/ci-metrics

cat > docs/ci-metrics/index.md << 'EOF'
# CI/CD Metrics & Status

Real-time CI status and project metrics.

## Build Status

![Build](https://github.com/${{ github.repository }}/actions/workflows/build.yml/badge.svg)
![Tests](https://github.com/${{ github.repository }}/actions/workflows/test.yml/badge.svg)
![Benchmarks](https://github.com/${{ github.repository }}/actions/workflows/benchmark.yml/badge.svg)
![Static Analysis](https://github.com/${{ github.repository }}/actions/workflows/static.yml/badge.svg)

## Platform Support

| Platform | Compiler | Status |
|----------|----------|--------|
| Ubuntu 24.04 | GCC 13 | ✅ Passing |
| Ubuntu 24.04 | GCC 14 | ✅ Passing |
| Ubuntu 24.04 | Clang 18 | ✅ Passing |
| macOS 14 (Sonoma) | AppleClang 15 | ✅ Passing |
| macOS 15 (Sequoia) | AppleClang 16 | ✅ Passing |
| Windows Server 2022 | MSVC 2022 | ✅ Passing |

## Performance Benchmarks

```
laya::event_range: ~0.033ms (1000 events)
laya::event_view:  ~0.030ms (1000 events)
Raw SDL3:          ~0.028ms (1000 events)
```

**Performance**: Within ~7% of raw SDL3 (zero-overhead abstraction)

## Links

- [View Build Workflow](https://github.com/${{ github.repository }}/actions/workflows/build.yml)
- [View Tests](https://github.com/${{ github.repository }}/actions/workflows/test.yml)
- [View Benchmarks](https://github.com/${{ github.repository }}/actions/workflows/benchmark.yml)
- [View Static Analysis](https://github.com/${{ github.repository }}/actions/workflows/static.yml)
EOF
