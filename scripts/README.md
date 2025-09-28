# Scripts

This directory contains useful scripts for building, testing, and general development. Some of these scripts are for local use, while others help with CI/CD workflows, health checks, and automation.

Every script includes internal documentation and usage instructions that you can view by opening the file or running it with `--help`.

## Developer Utilities

These scripts are intended to be used by developers working on the project. They help with common tasks like setting up the environment, running tests, and checking code quality.

### health/actions.sh

Checks if the local environment has `act` and `docker` installed - prerequisites for running GitHub Actions workflows locally. If one is missing, it provides installation instructions.

```bash
# Check if you have act and docker installed
./scripts/health/actions.sh

# Install missing dependencies for you
./scripts/health/actions.sh --install-missing
```

### Testing Workflows

Now you can run GitHub Actions workflows locally using `act`. Here are some useful commands:

```bash
act                # runs default workflow (on: push)
act pull_request   # simulate PR event
act -j build-linux # run a specific job
act --verbose      # more log detail
act --n            # list jobs, don’t run
```

## unix-health-check.sh

A comprehensive health check script for Unix-like systems. It verifies the presence and versions of essential development tools and libraries, ensuring your environment is correctly set up for building and running the project.

```bash
./scripts/unix_health_checks.sh

# To automatically install missing dependencies (requires sudo)
./scripts/unix_health_checks.sh --install-missing
```
