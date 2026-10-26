# VanillaPDF Python Bindings

[![PyPI](https://img.shields.io/pypi/v/vanillapdf)](https://pypi.org/project/vanillapdf/)
[![Python](https://img.shields.io/pypi/pyversions/vanillapdf)](https://pypi.org/project/vanillapdf/)
[![License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)

Python bindings for [Vanilla.PDF](https://github.com/vanillapdf/vanillapdf), a high-performance C++17 library for PDF document processing.

📖 **Documentation:** [vanillapdf-py.readthedocs.io](https://vanillapdf-py.readthedocs.io) — API reference and examples.

## Installation

```bash
pip install vanillapdf
```

Requires Python 3.11 or later. Pre-built wheels are available for Windows, Linux, and macOS.

## Quick Start

```python
import vanillapdf

# Open a document, read its title, and save a copy
with vanillapdf.Document.open("input.pdf") as doc:
    info = doc.get_document_info()
    if info is not None and info.title is not None:
        print("Title:", info.title.text)

    doc.save("output.pdf")
```

More runnable scripts — creating, encrypting, signing, and verifying documents — are in [`examples/`](examples/).

## Features

| Available Now | Coming Soon |
|---------------|-------------|
| Open, create, and save PDF documents | JPEG2000 and ICC profile support |
| Document metadata, pages, and low-level object traversal | |
| Encryption and decryption | |
| Digital signatures (signing and verification) | |
| Context manager support | |
| Cross-platform (Windows, Linux, macOS) | |

The Python bindings expose functionality from the [Vanilla.PDF C++ library](https://github.com/vanillapdf/vanillapdf), with more features being added progressively.

## Development

```bash
# Install with test dependencies
pip install -e .[test]

# Run tests
pytest

# Build from source (requires CMake and a C++ compiler)
pip install build
python -m build
```

## Contributing

Contributions are welcome. Please read the [contributing guidelines](https://github.com/vanillapdf/vanillapdf.py/blob/main/CONTRIBUTING.md) before submitting a pull request.

## License

Licensed under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0).

---

Developed by [Vanilla.PDF Labs](https://vanillapdf.com) — secure, high-performance PDF technology.
