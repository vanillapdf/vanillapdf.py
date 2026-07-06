"""Map :class:`RawFunction` facts onto Python-typed :class:`Function` signatures.

This is where PyArg format characters and native return tokens become Python
type annotations. It has no knowledge of C++ syntax or ``.pyi`` text.
"""
from __future__ import annotations

from .model import Function, Param, RawFunction

# PyArg_ParseTuple scalar format char -> (python type, preferred parameter name)
_SCALAR: dict[str, tuple[str, str]] = {
    "s": ("str", "text"),
    "n": ("int", "value"),
    "i": ("int", "value"),
    "L": ("int", "value"),
    "K": ("int", "value"),
    "k": ("int", "value"),
    "p": ("bool", "flag"),
    "d": ("float", "value"),
}


def _return_type(fn: RawFunction) -> str:
    tokens = set(fn.returns)
    optional = "Py_RETURN_NONE" in tokens

    # Delegating helpers with a known, fixed contract.
    if "get_string_property" in tokens:      # document info: Object capsule or None
        return "ObjectHandle | None"
    if "result_buffer" in tokens:            # signer name / message: Buffer or None
        return "BufferHandle | None"
    if "result_boolean" in tokens:
        return "bool"
    if "get_coordinate" in tokens:           # rectangle coordinates are bigint
        return "int"

    if fn.return_handle is not None:
        return f"{fn.return_handle} | None" if optional else fn.return_handle
    if {"Py_RETURN_TRUE", "Py_RETURN_FALSE"} & tokens:
        return "bool"
    if "PyUnicode_FromString" in tokens:
        return "str"
    if "PyBytes_FromStringAndSize" in tokens:
        return "bytes"
    if "to_python" in tokens:
        return "float" if fn.name.startswith("real_object") else "int"
    if "capsule_release" in tokens or optional:
        return "None"
    raise SystemExit(f"unclassified return for {fn.name}: {fn.returns}")


def _params(fn: RawFunction) -> tuple[Param, ...]:
    # First pass: (annotation, base name), consuming one &out var per format char.
    slots: list[tuple[str, str]] = []
    var_index = 0
    position = 0
    i = 0
    while i < len(fn.fmt):
        char = fn.fmt[i]
        if char == "O":
            var = fn.varnames[var_index] if var_index < len(fn.varnames) else ""
            var_index += 1
            arg = fn.handle_args.get(var)
            if arg is None:  # parser guarantees this; guard anyway rather than guess
                raise SystemExit(f"{fn.name}: capsule arg {var!r} has no resolved handle type")
            annotation = arg.handle_type + (" | None" if arg.optional else "")
            base = "handle" if position == 0 and "Handle" in annotation else "other"
            slots.append((annotation, base))
        elif char in _SCALAR:
            var_index += 1
            slots.append(_SCALAR[char])  # (annotation, base)
        elif char == "y":
            var_index += 1
            if i + 1 < len(fn.fmt) and fn.fmt[i + 1] == "#":
                var_index += 1
                i += 1
            slots.append(("bytes", "data"))
        elif char == "#":
            pass
        else:
            raise SystemExit(f"unhandled PyArg format char {char!r} in {fn.fmt!r}")
        position += 1
        i += 1

    # Second pass: only disambiguate names with an index when there is more than
    # one parameter (the sole handle keeps the bare name "handle").
    total = len(slots)
    params: list[Param] = []
    for j, (annotation, base) in enumerate(slots):
        name = base if total == 1 or (base == "handle" and j == 0) else f"{base}{j}"
        params.append(Param(name, annotation))
    return tuple(params)


def to_functions(raw: list[RawFunction]) -> list[Function]:
    """Convert parsed facts into rendered-ready, Python-typed signatures."""
    return [Function(fn.name, _params(fn), _return_type(fn))
            for fn in sorted(raw, key=lambda f: f.name)]
