# VanillaPDF Python Bindings

[![PyPI](https://img.shields.io/pypi/v/vanillapdf)](https://pypi.org/project/vanillapdf/)
[![Python](https://img.shields.io/pypi/pyversions/vanillapdf)](https://pypi.org/project/vanillapdf/)
[![License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)

Python bindings for [Vanilla.PDF](https://github.com/vanillapdf/vanillapdf), a high-performance C++17 library for PDF document processing.

## Installation

```bash
pip install vanillapdf
```

Requires Python 3.8 or later. Pre-built wheels are available for Windows, Linux, and macOS.

## Quick Start

```python
import vanillapdf

# Open and save a document
with vanillapdf.Document("input.pdf") as doc:
    doc.save("output.pdf")

# Work with in-memory buffers
with vanillapdf.Buffer.create() as buf:
    buf.set_data(b"binary data")
    data = buf.get_data()
```

## Features

| Available Now | Coming Soon |
|---------------|-------------|
| Open and save PDF documents | Create new PDF documents |
| In-memory buffer operations | Digital signatures |
| Context manager support | Encryption and decryption |
| Cross-platform (Windows, Linux, macOS) | JPEG2000 and ICC profile support |

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

Contributions are welcome. Please read the [contributing guidelines](https://github.com/vanillapdf/vanillapdf/blob/main/CONTRIBUTING.md) before submitting a pull request.

## License

Licensed under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0).

---

Developed by [Vanilla.PDF Labs](https://vanillapdf.com) — secure, high-performance PDF technology.
