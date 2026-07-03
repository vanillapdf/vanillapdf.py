"""Read a document's information dictionary (title, author, producer, ...).

Usage::

    python read_metadata.py document.pdf
"""

import sys

from vanillapdf import Document

# The document info entries are optional: a missing entry is returned as None
# rather than raising, so guard each one before decoding it.
FIELDS = ("title", "author", "subject", "keywords", "creator", "producer")


def read_metadata(path: str) -> None:
    with Document(path) as doc:
        info = doc.get_document_info()
        if info is None:
            print(f"{path}: no document information dictionary")
            return

        for name in FIELDS:
            entry = getattr(info, name)
            if entry is None:
                continue
            # Each entry is a StringObject; value_string() decodes its bytes.
            print(f"{name.capitalize():10}: {entry.value_string()}")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("usage: python read_metadata.py document.pdf")
    read_metadata(sys.argv[1])
