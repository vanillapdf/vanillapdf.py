"""Process a PDF entirely in memory: bytes in, bytes out.

This is the shape of a web handler or a pipeline step, where the PDF arrives as
bytes (an upload, an HTTP or S3 body) and must be returned as bytes, without a
temporary file. Here the bytes come from and go to files only to make the
script runnable.

Usage::

    python in_memory.py input.pdf output.pdf
"""

import sys

from vanillapdf import Document, PageObject, Rectangle


def add_blank_page(data: bytes) -> bytes:
    # open_bytes copies the data, so the caller's bytes can be dropped at once.
    with Document.open_bytes(data) as doc:
        page = PageObject.create_from_document(doc)

        # US Letter media box (points): 612 x 792.
        rect = Rectangle.create()
        rect.lower_left_x = 0
        rect.lower_left_y = 0
        rect.upper_right_x = 612
        rect.upper_right_y = 792
        page.media_box = rect

        doc.get_catalog().get_pages().append(page)
        return doc.save_bytes()


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise SystemExit("usage: python in_memory.py input.pdf output.pdf")

    with open(sys.argv[1], "rb") as source:
        result = add_blank_page(source.read())

    with open(sys.argv[2], "wb") as target:
        target.write(result)

    print(f"Wrote {len(result)} bytes to {sys.argv[2]}")
