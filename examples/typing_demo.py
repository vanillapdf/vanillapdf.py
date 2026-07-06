"""Typing demo — open this in an editor with a Python language server to see
vanillapdf's types in action.

- **VS Code**: Pylance works out of the box.
- **Sublime Text**: install Package Control, then the ``LSP`` and ``LSP-pyright``
  packages, with the interpreter pointed at a venv that has ``vanillapdf``
  installed.

Nothing here needs to *run* — it exists for the editor / type checker. Try:

- **hover** a value (e.g. ``doc.get_catalog()``) to see its type,
- type a ``.`` after a value to get **autocomplete** of that object's members,
- **uncomment** the deliberate error near the bottom and watch it get flagged.

Every annotated local below is also a lightweight regression test: if a public
return type ever changed, the assignment would fail pyright (and CI).
"""
from __future__ import annotations

from vanillapdf import (
    Catalog,
    Document,
    DocumentInfo,
    IntegerObject,
    PageObject,
    PageTree,
    Rectangle,
    StringObject,
)


def walk(doc: Document) -> None:
    catalog: Catalog = doc.get_catalog()                  # hover -> Catalog
    pages: PageTree = catalog.get_pages()                 # autocomplete after `catalog.`
    page: PageObject = pages[0]                           # indexing yields PageObject
    media_box: Rectangle = page.media_box
    width: int = media_box.width                          # coordinates are int, not Unknown

    info: DocumentInfo | None = doc.get_document_info()   # optional: needs a None check
    if info is not None:
        title: StringObject | None = info.title
        print(title)

    print(width)

    # Uncomment to watch the type checker complain
    # ("Rectangle" is not assignable to "int"):
    # oops: int = page.media_box


def scalars() -> None:
    value: int = IntegerObject.create(5).value           # .value is int, not Unknown
    print(value)
