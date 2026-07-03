"""Walk a document's page tree and print each page's media box.

Usage::

    python inspect_pages.py document.pdf
"""

import sys

from vanillapdf import Document


def inspect_pages(path: str) -> None:
    with Document(path) as doc:
        pages = doc.get_catalog().get_pages()
        print(f"{path}: {len(pages)} page(s)")

        # PageTree is 0-based and iterable, just like a Python sequence.
        for index, page in enumerate(pages):
            box = page.media_box
            print(
                f"  page {index}: "
                f"[{box.lower_left_x} {box.lower_left_y} "
                f"{box.upper_right_x} {box.upper_right_y}] "
                f"({box.width} x {box.height})"
            )


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("usage: python inspect_pages.py document.pdf")
    inspect_pages(sys.argv[1])
