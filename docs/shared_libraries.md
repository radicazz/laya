# Shared Library Setup

This document explains how laya is configured to build and deploy shared libraries for optimal runtime distribution.

## Overview

When `LAYA_BUILD_SHARED=ON` (the default), laya builds as a shared library along with all its SDL dependencies. This provides several advantages:

- **Smaller executables** - Multiple programs can share the same library instances
- **Runtime updates** - Libraries can be updated independently of executables
- **Better development workflow** - Faster incremental builds and testing

## Current Configuration

### Libraries Built as Shared

- **laya.dll** (~88 KB) - Main laya library
- **SDL3.dll** (~6.3 MB) - Core SDL3 functionality
- **SDL3_image.dll** (~492 KB) - Image loading support
- **SDL3_ttf.dll** (~5.8 MB) - Font rendering support

### Automatic Deployment

The build system automatically copies all required shared libraries to executable directories:

```bash
examples/Debug/
├── hello_laya.exe
├── laya.dll
├── SDL3.dll
├── SDL3_image.dll
└── SDL3_ttf.dll

tests/Debug/
├── laya_tests.exe
├── laya.dll
├── SDL3.dll
├── SDL3_image.dll
└── SDL3_ttf.dll
```

## Platform-Specific Behavior

### Windows

- DLL files are copied to the same directory as the executable
- Uses automatic `WINDOWS_EXPORT_ALL_SYMBOLS` for easier development
- Generated export headers for clean API boundaries

### Linux/macOS

- Shared libraries (.so/.dylib) are copied to executable directory  
- `RPATH` is set to `$ORIGIN` (Linux) or `@loader_path` (macOS)
- This allows executables to find libraries in the same directory
- No need to modify system `LD_LIBRARY_PATH` or similar
- **Self-contained builds**: Uses vendored FreeType, HarfBuzz, libtiff, libpng
- **Minimal system dependencies**: Only requires basic graphics/audio system libs

## Build Commands

```bash
# Configure with shared libraries (default)
cmake -B build -DLAYA_BUILD_SHARED=ON

# Build everything
cmake --build build --config Debug

# Run executables (libraries are automatically found)
./build/examples/Debug/hello_laya      # Linux/macOS
.\build\examples\Debug\hello_laya.exe  # Windows
```

## For Developers

### Adding New Executables

To ensure new executables get the required shared libraries, add this to your CMakeLists.txt:

```cmake
add_executable(my_app my_app.cpp)
target_link_libraries(my_app PRIVATE laya::laya)

# Copy shared libraries for runtime
laya_copy_shared_libs(my_app)
```

### Static Linking (Alternative)

If you prefer static linking for distribution:

```bash
cmake -B build -DLAYA_BUILD_SHARED=OFF
```

This creates larger executables but with no external dependencies.

## GitHub Actions Integration

All CI workflows automatically build with shared libraries:

- **Ubuntu builds**: Run on all branches with GCC and Clang compilers
- **Windows builds**: Run only on master branch with MSVC compiler  
- **Headless testing**: Uses Xvfb virtual display for graphics-free CI
- **Faster builds**: Incremental compilation and smart caching
- Tests run with the same library configuration as examples
- Release artifacts include all necessary DLLs/shared libraries
- **Dependency verification**: Post-build `ldd` checks ensure proper linking

## Distribution

When distributing applications:

1. **Include all DLL/shared library files** alongside your executable
2. **Maintain directory structure** - keep libraries in the same folder as the executable
3. **No additional installation** required on target systems (libraries are self-contained)

## Troubleshooting

### "DLL not found" errors

- Verify all DLL files are in the same directory as the executable
- Check that `laya_copy_shared_libs()` was called for your target

### Linux: "library not found" errors

- Verify RPATH is set correctly: `ldd ./my_executable`
- Should show libraries resolved to `$ORIGIN/lib*.so`

### Build system not copying libraries

- Ensure you're building with `LAYA_BUILD_SHARED=ON`
- Verify your executable target calls `laya_copy_shared_libs(target_name)`

### Ubuntu/Debian missing system dependencies

```bash
# Essential runtime packages (usually pre-installed on desktop systems)
sudo apt-get install -y \
  libasound2 libpulse0 \
  libx11-6 libxext6 libxrandr2 libxcursor1 \
  libgl1-mesa-glx libegl1-mesa \
  libdbus-1-3

# For headless/server systems (CI/testing without display)
sudo apt-get install -y xvfb
export DISPLAY=:99
Xvfb :99 -screen 0 1024x768x24 > /dev/null 2>&1 &
```

### Deployment on clean Ubuntu systems

Our shared libraries are **mostly self-contained** but may require these minimal system packages:

**Audio**: `libasound2`, `libpulse0`  
**Graphics**: `libx11-6`, `libgl1-mesa-glx`, `libegl1-mesa`  
**System**: `libdbus-1-3`, `libc6`

These are typically pre-installed on desktop Ubuntu but may need installation on minimal/server images.
