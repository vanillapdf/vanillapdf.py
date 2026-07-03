"""Create a new PDF document with a single blank page and save it.

Usage::

    python create_document.py [output.pdf]
"""

import sys

import vanillapdf
from vanillapdf import Document, PageObject, Rectangle


def create_document(path: str) -> None:
    # A document opened for creation owns the whole object graph; closing it
    # (here via the ``with`` block) releases every handle we obtain from it.
    with Document.create(path) as doc:
        page = PageObject.create_from_document(doc)

        # US Letter media box (points): 612 x 792.
        rect = Rectangle.create()
        rect.lower_left_x = 0
        rect.lower_left_y = 0
        rect.upper_right_x = 612
        rect.upper_right_y = 792
        page.media_box = rect

        doc.get_catalog().get_pages().append(page)
        doc.save(path)

    print(f"Created {path} (vanillapdf {vanillapdf.__version__})")


if __name__ == "__main__":
    output = sys.argv[1] if len(sys.argv) > 1 else "output.pdf"
    create_document(output)
