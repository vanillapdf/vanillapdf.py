# VanillaPDF Python Bindings

&#x20;&#x20;

**VanillaPDF** is a Python wrapper around [Vanilla.PDF](https://github.com/vanillapdf/vanillapdf), a modern C++17 library for creating, editing, and signing PDF documents.

---

## 🚀 Installation

```bash
pip install vanillapdf
```

Or install from source (requires CMake, Python ≥3.8):

```bash
pip install git+https://github.com/vanillapdf/vanillapdf.git
```

---

## 📚 Usage Example

```python
import vanillapdf

doc = vanillapdf.Document()
doc.load("sample.pdf")
doc.save("output.pdf")
```

---

## 📦 Features

* Create and modify PDF documents in pure C++
* Digital signatures and encryption
* JPEG2000, LZW, ICC profile support
* Native performance with Python bindings
* Cross-platform: Windows, Linux, macOS

---

## 🧪 Testing

To run tests locally:

```bash
pip install -e .[test]
pytest
```

---

## 🔧 Build from Source

VanillaPDF is built using CMake and vcpkg.

```bash
pip install build
python -m build
```

You can also use `cibuildwheel` to generate wheels for distribution.

---

## 🤝 Contributing

Contributions are welcome! Please read the [contributing guidelines](https://github.com/vanillapdf/vanillapdf/blob/main/CONTRIBUTING.md) first.

---

## 🧓️ License

Licensed under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0).

---

## 🏢 About Vanilla.PDF Labs

VanillaPDF is developed and maintained by [Vanilla.PDF Labs](https://vanillapdf.com/about), a company focused on secure, high-performance PDF technology.

For more information, visit [vanillapdf.com](https://vanillapdf.com).
