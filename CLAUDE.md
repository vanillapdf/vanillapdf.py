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

The `_vanillapdf` module bridges Python to the C++ library. Sources are **C++**
(`.cpp`) and organized to mirror the upstream C API layout — one file per object
type in subdirectories:

- `vanillapdfmodule.cpp` - Module definition + method table
- `common.cpp/.h` - Shared infrastructure: capsule helpers (`capsule_new/get/
  release`, `capsule_cast<T>`), `raise_last_error` + the `PdfError` exception,
  `ScopeGuard`, `py_malloc<T>`, `narrow_unsigned<T>`
- `utils/` - buffer, errors, logging, library_info, license_info, misc_utils
- `syntax/` - object (+ `object_cast<T>` helper), file, and one file per object
  type (integer_object, boolean_object, string_object, ...)
- `semantics/` - document (and higher-level PDF constructs)

Includes are resolved from the native source root (e.g. `#include
"syntax/object.h"`), configured via `target_include_directories` in CMake.
Native library handles are wrapped as PyCapsule objects; every syntax object is
exposed as a single `"VanillaPDF.Object"` capsule, with typed accessors
converting to concrete handles internally via `X_FromObject`.

See [`docs/STYLE.md`](docs/STYLE.md) for the coding conventions (casts, error
handling, handles/memory, Python wrappers).

### Build System

- **CMake** with **scikit-build-core** for Python packaging
- **vcpkg** manages C++ dependencies, fetched via CMake `FetchContent` (no submodule)
- The vanillapdf C++ library itself is also fetched via `FetchContent` (pinned tag in `CMakeLists.txt`)
- Static linking by default; dynamic linking available via `BUILD_SHARED_LIBS`

### Key Files

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Build configuration, defines `_vanillapdf` module, pins vcpkg + vanillapdf versions |
| `pyproject.toml` | Python packaging, scikit-build-core config |
| `vcpkg.json` | Declares the C++ dependencies this project needs from vcpkg |

## Testing

Test files in `tests/` use `assets/pdf-test.pdf` for document operations:

```bash
pytest tests/                    # Run all tests
pytest tests/test_document.py    # Run single test file
pytest tests/ -v                 # Verbose output
```

### Zero tolerance for flakiness

Flaky tests are treated as **bugs, not noise**. This project must not ship any
non-deterministic test behaviour.

- **Never assume a re-run fixes a failure.** A test that fails then passes on
  re-run is a defect to be root-caused immediately — inspect logs, error
  messages, crash dumps, allocator diagnostics, whatever it takes. Do not
  proceed until the cause is understood and eliminated.
- **Debug immediately and empirically.** Reproduce in a loop, bisect the
  trigger (isolate operations/tests, vary one factor at a time), and prove the
  fix by hammering the previously-failing case many times (dozens of runs).
- **Fix the trigger, don't mask it.** Removing the actual cause (see below) is a
  fix; loosening an assertion or retrying is masking and is not acceptable.

Known native-library bug (root-caused and isolated). The failure needs **both**
the native library's logging enabled **and** pytest's default `--capture=fd`.
The library logs to **stdout (fd 1)**; `--capture=fd` `dup2`s a temp file over
fd 1 and `ftruncate`s it between tests, and truncating that file underneath the
library's logging corrupts its I/O state, which bleeds into the parser as
spurious `PARSE_EXCEPTION` ("Could not parse object at offset N"). It is **not**
the file (byte-valid) and **not** a PyMem/bindings issue (Python debug allocator
clean). Reproduced **without pytest** in `repro/logging_fd_capture_parse_bug.py`
(logging on → intermittent failures; `--off` → 0). **This must be fixed in the
native C++ library.** Until then, `tests/conftest.py` disables native logging
per-test as a **workaround** (documented there) — it removes the trigger; it is
not a fix and must be removed once the native bug is resolved.

### Linting

Ruff enforces style/quality (config in `pyproject.toml`, incl. `SIM117`
multiple-with-statements). Run `ruff check .`; CI runs it in `lint.yml`.

## CI/CD

- **sanity-check.yml** - Runs on push/PR to main; builds and tests on Ubuntu, Windows, macOS
- **release.yml** - Manual dispatch; builds wheels for Python 3.10-3.14 and publishes to PyPI
