"""Command-line entry point for the native stub generator."""
from __future__ import annotations

import argparse
import os
import sys

from . import build_stub
from .sources import REPO_ROOT, STUB_PATH


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        prog="generate_native_stub",
        description="Generate src/vanillapdf/_vanillapdf.pyi from the native sources.")
    parser.add_argument(
        "--check", action="store_true",
        help="exit non-zero if the committed stub is out of date (for CI)")
    args = parser.parse_args(argv)

    rendered = build_stub()
    relpath = os.path.relpath(STUB_PATH, REPO_ROOT)

    if args.check:
        current = ""
        if os.path.exists(STUB_PATH):
            with open(STUB_PATH, encoding="utf-8") as fh:
                current = fh.read()
        if current != rendered:
            print(f"ERROR: {relpath} is out of date; run "
                  "`python scripts/generate_native_stub.py`", file=sys.stderr)
            return 1
        print(f"{relpath} is up to date.")
        return 0

    with open(STUB_PATH, "w", encoding="utf-8") as fh:
        fh.write(rendered)
    functions = rendered.count("\ndef ")
    handles = rendered.count("\nclass ") - 1  # minus PdfError
    print(f"Wrote {relpath} ({functions} functions, {handles} handle types).")
    return 0
