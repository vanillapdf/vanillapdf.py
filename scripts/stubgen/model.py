"""Typed data model shared across the stub generator.

The pipeline is ``parser`` -> :class:`RawFunction` (facts scraped from the C++
source) -> ``pytypes`` -> :class:`Function` (Python-typed signature) -> ``renderer``.
Nothing here knows about regexes or ``.pyi`` text; it is pure data.
"""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class HandleArg:
    """A native capsule argument and the marker handle type it must receive."""

    handle_type: str          # e.g. "BufferHandle", "ObjectHandle"
    optional: bool = False    # True when the native code accepts a null capsule


@dataclass(frozen=True)
class RawFunction:
    """Facts extracted from one native ``PyMethodDef`` function body.

    This is the boundary between the C++-specific parser and the
    Python-type mapping; it holds no ``.pyi`` syntax.
    """

    name: str
    fmt: str                                  # PyArg_ParseTuple format string
    varnames: tuple[str, ...]                 # the &out variables, in order
    returns: tuple[str, ...]                  # return-expression tokens seen
    handle_args: dict[str, HandleArg]         # 'O' varname -> its handle type
    return_handle: str | None                 # marker type returned, if any


@dataclass(frozen=True)
class Param:
    """A single rendered parameter, e.g. ``handle: BufferHandle``."""

    name: str
    annotation: str

    def render(self) -> str:
        return f"{self.name}: {self.annotation}"


@dataclass(frozen=True)
class Function:
    """A fully Python-typed native function signature, ready to render."""

    name: str
    params: tuple[Param, ...]
    return_type: str

    def render(self) -> str:
        params = ", ".join(p.render() for p in self.params)
        return f"def {self.name}({params}) -> {self.return_type}: ..."
