"""Generate ``src/vanillapdf/_vanillapdf.pyi`` from the native C extension.

The native ``_vanillapdf`` module is a compiled C extension, so a type checker
sees no information for it -- every ``_vanillapdf.*`` call would be ``Unknown``.
This package reads the native ``src/vanillapdf_native/**/*.cpp`` sources and
emits a type stub so the Python wrappers can be checked under pyright strict
(see issue #42).

Handles are opaque, named ``PyCapsule`` objects. The binding validates each
capsule's name at every boundary (``capsule_cast``/``object_cast``), so a
mismatched handle raises ``TypeError``/``PdfError`` at runtime. Giving each
native C++ handle type a distinct marker class in the stub moves that check to
pyright, catching a wrong handle in the editor/CI before it becomes a runtime
error. Syntax objects are the one exception: they share a single "Object"
capsule and are validated by ``object_cast`` (which checks the object type), so
they all use the common ``ObjectHandle``.

Pipeline::

    sources.read_sources()        # native .cpp text
      -> parser.parse()           # -> [RawFunction]        (C++ facts)
      -> pytypes.to_functions()   # -> [Function]           (Python types)
      -> renderer.render()        # -> str                  (.pyi text)

The parser fails loudly (``SystemExit``) on anything it cannot classify, so a
missed pattern is a build error rather than a silently wrong handle type that
would only surface as a runtime ``TypeError``.
"""
from __future__ import annotations

from . import parser, pytypes, renderer, sources
from .sources import STUB_PATH

__all__ = ["build_stub", "STUB_PATH"]


def build_stub() -> str:
    """Read the native sources and return the rendered ``.pyi`` text."""
    functions = pytypes.to_functions(parser.parse(sources.read_sources()))
    return renderer.render(functions)
