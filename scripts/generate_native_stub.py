#!/usr/bin/env python
"""Entry point for the native ``_vanillapdf`` type-stub generator.

The implementation lives in the :mod:`stubgen` package next to this file; this
wrapper just makes it runnable as a plain script (and via ``--check`` in CI)::

    python scripts/generate_native_stub.py            # (re)write the stub
    python scripts/generate_native_stub.py --check    # fail if out of date

See ``scripts/stubgen/__init__.py`` for how the native sources are parsed.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from stubgen.cli import main  # noqa: E402  (after sys.path setup)

if __name__ == "__main__":
    raise SystemExit(main())
