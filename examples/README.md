# Examples

Runnable scripts demonstrating the VanillaPDF Python bindings. Each is
self-contained and also embedded in the documentation (`docs/examples.rst`).

Install the package first (`pip install -e .` from the repo root), then:

```bash
python examples/create_document.py output.pdf         # create a blank one-page PDF
python examples/read_metadata.py assets/pdf-test.pdf  # print the info dictionary
python examples/inspect_pages.py assets/pdf-test.pdf  # list pages + media boxes
python examples/traverse_objects.py assets/pdf-test.pdf 2 0  # resolve object 2 0
python examples/encrypt_document.py plain.pdf encrypted.pdf # AES-128 encrypt
```

| Script | Layer | Shows |
|--------|-------|-------|
| `create_document.py` | semantic | Create a document, add a page with a media box, save |
| `read_metadata.py` | semantic | Read optional document-info entries (None when absent) |
| `inspect_pages.py` | semantic | Traverse the 0-based page tree, read media boxes |
| `traverse_objects.py` | syntax | Initialize a `File`, resolve an indirect object, walk a dictionary |
| `encrypt_document.py` | semantic | Encrypt a document with AES-128 + user/owner passwords |
