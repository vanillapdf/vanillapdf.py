# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

VanillaPDF Python Bindings - Python wrapper around the [Vanilla.PDF](https://github.com/vanillapdf/vanillapdf) C++17 library for creating, editing, and signing PDF documents.

## Build Commands

```bash
# Install in editable mode (development)
pip install -e .

# Install with test dependencies
pip install -e .[test]

# Run tests
pytest tests/

# Build wheel for distribution
pip install build
python -m build --wheel
```

### Platform-Specific Setup

**Windows:**
```bash
python -m venv venv
venv\Scripts\activate
pip install -r requirements.txt
pip install -e .
```

**Linux:**
```bash
sudo apt install -y build-essential cmake ninja-build python3-dev git curl unzip pkg-config
python3 -m venv venv
source venv/bin/activate
pip install -e .
```

## Architecture

### Layer Structure

```
vanillapdf (Python package)
    ├── Document, Buffer classes (src/vanillapdf/*.py)
    └── _vanillapdf (C extension module)
            └── vanillapdf C++ library (via vcpkg)
```

### Python API (`src/vanillapdf/`)

- **Document** - Open, save, and manage PDF documents
- **Buffer** - In-memory data buffer for PDF operations

Both classes support context managers and require explicit `close()` or `with` statements for resource cleanup.

### C Extension (`src/vanillapdf_native/`)

The `_vanillapdf` module bridges Python to the C++ library:
- `vanillapdfmodule.c` - Main module definition
- `documentmodule.c` - Document operations (open, save, release)
- `buffermodule.c` - Buffer operations (create, get/set data, release)

C++ handles are wrapped as PyCapsule objects for safe passing between Python and C.

### Build System

- **CMake** with **scikit-build-core** for Python packaging
- **vcpkg** (in `external/vcpkg/`) manages C++ dependencies
- Custom vcpkg port in `vcpkg-ports/vanillapdf/`
- Static linking by default; dynamic linking available via `BUILD_SHARED_LIBS`

### Key Files

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Build configuration, defines `_vanillapdf` module |
| `pyproject.toml` | Python packaging, scikit-build-core config |
| `cmake/vcpkg_init.cmake` | Auto-bootstraps vcpkg if missing |
| `vcpkg.json` | Declares vanillapdf C++ dependency |

## Testing

Test files in `tests/` use `assets/pdf-test.pdf` for document operations:

```bash
pytest tests/                    # Run all tests
pytest tests/test_document.py    # Run single test file
pytest tests/ -v                 # Verbose output
```

## CI/CD

- **sanity-check.yml** - Runs on push/PR to main; builds and tests on Ubuntu, Windows, macOS
- **release.yml** - Manual dispatch; builds wheels for Python 3.8-3.13 and publishes to PyPI
