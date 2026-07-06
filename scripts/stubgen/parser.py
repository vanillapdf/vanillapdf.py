"""Scrape native ``PyMethodDef`` functions into :class:`RawFunction` facts.

All C++-specific regex lives here. The rest of the generator works only on the
typed :mod:`stubgen.model` objects this module produces.
"""
from __future__ import annotations

import re

from .model import HandleArg, RawFunction
from .sources import Source, body_from

# A registered function:  PyObject* name(PyObject* self, PyObject* args) { ... }
_FUNCTION = re.compile(
    r"PyObject\s*\*\s*([a-z0-9_]+)\s*\(\s*PyObject\s*\*\s*\w+\s*,"
    r"\s*PyObject\s*\*\s*\w+\s*\)\s*\{", re.M)
# A shared body helper the function may tail-delegate to (takes `args`).
_HELPER = re.compile(
    r"static\s+PyObject\s*\*\s*([a-z0-9_]+)\s*\(\s*PyObject\s*\*\s*args\s*,", re.M)
# `<x>_capsule_from_handle(<Type>Handle* ...)` -> the marker type it wraps.
_CAPSULE_HELPER = re.compile(
    r"PyObject\s*\*\s*([a-z0-9_]+_capsule_from_handle)\s*\(\s*(\w+?)Handle\s*\*")
# Capsule-name constant -> handle type, learned from cast and creation sites:
#   capsule_cast<BufferHandle>(x, BUFFER_CAPSULE)
#   capsule_new(x, SETTINGS_CAPSULE, RELEASE_FN(DocumentEncryptionSettings_Release))
# Constant names may contain digits (e.g. PKCS12_KEY_CAPSULE).
_CONST_FROM_CAST = re.compile(r"capsule_cast<\s*(\w+?)Handle\s*>\(\s*\w+\s*,\s*([A-Z0-9_]+)\s*\)")
_CONST_FROM_NEW = re.compile(
    r"capsule_new\(\s*\w+\s*,\s*([A-Z0-9_]+)\s*,\s*RELEASE_FN\(\s*(\w+?)_Release\s*\)")
# Method table entry:  {"name", func, METH_FLAG, ...}
_METHOD = re.compile(
    r'\{\s*"([a-z0-9_]+)"\s*,\s*[A-Za-z_][A-Za-z0-9_]*\s*,\s*METH_(\w+)')
# PyArg_ParseTuple(args, "fmt", &v0, &v1, ...)
_PARSE = re.compile(r'PyArg_ParseTuple\(\s*args\s*,\s*"([^"]*)"\s*((?:,\s*&\w+\s*)*)\)')
# Any static helper definition, capturing its name and the '(' of its parameter list.
_STATIC_HELPER = re.compile(r"static\s+[\w:]+(?:\s*\*+)?\s+([a-z_]\w*)\s*\(", re.M)


def _paren_content(text: str, open_paren: int) -> tuple[str, int]:
    """Return (inner text, close-paren index) for the ``(...)`` at ``open_paren``."""
    depth = 0
    for i in range(open_paren, len(text)):
        if text[i] == "(":
            depth += 1
        elif text[i] == ")":
            depth -= 1
            if depth == 0:
                return text[open_paren + 1:i], i
    return "", open_paren


def _split_top_level(args: str) -> list[str]:
    """Split a C++ argument list on top-level commas (ignoring <...> and (...))."""
    out, depth, current = [], 0, []
    for ch in args:
        if ch in "<(":
            depth += 1
        elif ch in ">)":
            depth -= 1
        if ch == "," and depth == 0:
            out.append("".join(current).strip())
            current = []
        else:
            current.append(ch)
    if "".join(current).strip():
        out.append("".join(current).strip())
    return out


def _param_name(param: str) -> str:
    """The declared name of a C++ parameter (its trailing identifier)."""
    idents = re.findall(r"[A-Za-z_]\w*", param)
    return idents[-1] if idents else ""


def _direct_handle(body: str, var: str, capsule_types: dict[str, str]) -> HandleArg | None:
    """Resolve ``var`` from a cast written directly in ``body`` (no helper following)."""
    m = re.search(rf"capsule_cast<\s*(\w+?)Handle\s*>\(\s*{re.escape(var)}\b", body)
    if m:
        return HandleArg(m.group(1) + "Handle")
    # Every syntax object shares one "Object" capsule (safe object_cast).
    if re.search(rf"object_cast<[^>]+>\(\s*{re.escape(var)}\b", body):
        return HandleArg("ObjectHandle")
    # optional_handle(<var>, CAPSULE, &<out>): type is on the out declaration,
    # and a null capsule is accepted.
    om = re.search(rf"optional_handle\(\s*{re.escape(var)}\b[^;]*?&(\w+)\)", body)
    if om:
        dm = re.search(rf"(\w+?)Handle\s*\*\s*{re.escape(om.group(1))}\b", body)
        if dm:
            return HandleArg(dm.group(1) + "Handle", optional=True)
    # release/get functions identify the capsule only by its name constant
    # (e.g. capsule_release(capsule, BUFFER_CAPSULE)).
    cm = re.search(rf"capsule_(?:release|get)\(\s*{re.escape(var)}\s*,\s*([A-Z0-9_]+)\s*\)", body)
    if cm and cm.group(1) in capsule_types:
        return HandleArg(capsule_types[cm.group(1)])
    return None


def _handle_arg(body: str, var: str, capsule_types: dict[str, str],
                helpers: dict[str, tuple[list[str], str]]) -> HandleArg | None:
    """Resolve the marker handle type a single ``O`` argument must receive.

    Tries a cast written directly in ``body`` first, then follows one level into
    an extraction helper (e.g. ``extract_dictionary_and_key(capsule, key, ...)``)
    by mapping the call-site position to the helper's parameter.
    """
    direct = _direct_handle(body, var, capsule_types)
    if direct is not None:
        return direct
    for hname, (param_names, hbody) in helpers.items():
        for call in re.finditer(rf"\b{re.escape(hname)}\s*\(([^;{{}}]*)\)", body):
            call_args = _split_top_level(call.group(1))
            if var in call_args:
                index = call_args.index(var)
                if index < len(param_names):
                    via = _direct_handle(hbody, param_names[index], capsule_types)
                    if via is not None:
                        return via
    return None


def _return_handle(body: str, capsule_helpers: dict[str, str]) -> str | None:
    """Resolve the marker handle type a function returns, or None if not a handle."""
    if re.search(r"return\s+object_capsule_from_handle\s*\(", body):
        return "ObjectHandle"
    m = re.search(r"return\s+(\w+_capsule_from_handle)\s*\(", body)
    if m and m.group(1) in capsule_helpers:
        return capsule_helpers[m.group(1)]
    m = re.search(r"capsule_new\([^,]+,\s*\w+,\s*RELEASE_FN\(\s*(\w+?)_Release\s*\)", body)
    if m:
        return m.group(1) + "Handle"
    return None


def _capsule_varnames(fmt: str, varnames: list[str]) -> list[str]:
    """The subset of ``&out`` variables that correspond to ``O`` (capsule) args."""
    out: list[str] = []
    vi = 0
    i = 0
    while i < len(fmt):
        c = fmt[i]
        if c == "#":
            i += 1
            continue
        if c == "O" and vi < len(varnames):
            out.append(varnames[vi])
        if c == "y" and i + 1 < len(fmt) and fmt[i + 1] == "#":
            vi += 1  # y# consumes an extra length variable
            i += 1
        vi += 1
        i += 1
    return out


def _scan_body(body: str):
    """Return (fmt, varnames, return-tokens, delegate-helper-name) for a body."""
    parse = _PARSE.search(body)
    fmt = parse.group(1) if parse else None
    varnames = re.findall(r"&(\w+)", parse.group(2)) if parse else []
    returns = set(re.findall(r"return\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(", body))
    returns |= set(re.findall(r"(Py_RETURN_\w+)", body))
    delegated = re.search(r"return\s+([a-z0-9_]+)\s*\(\s*args\b", body)
    return fmt, varnames, returns, (delegated.group(1) if delegated else None)


def parse(sources: list[Source]) -> list[RawFunction]:
    """Extract every registered native function as a :class:`RawFunction`."""
    methods: dict[str, str] = {}
    global_capsule_helpers: dict[str, str] = {}
    # OBJECT_CAPSULE is shared by every syntax object (see object_cast).
    global_capsule_types: dict[str, str] = {"OBJECT_CAPSULE": "ObjectHandle"}
    for src in sources:
        for m in _METHOD.finditer(src.text):
            methods[m.group(1)] = m.group(2)
        for m in _CAPSULE_HELPER.finditer(src.text):
            global_capsule_helpers.setdefault(m.group(1), m.group(2) + "Handle")
        for m in _CONST_FROM_CAST.finditer(src.text):
            global_capsule_types.setdefault(m.group(2), m.group(1) + "Handle")
        for m in _CONST_FROM_NEW.finditer(src.text):
            global_capsule_types.setdefault(m.group(1), m.group(2) + "Handle")

    functions: list[RawFunction] = []
    for src in sources:
        # File-local mappings win over global: the shared SETTINGS_CAPSULE resolves
        # to a different handle type in each settings file, so a release function
        # must be typed from its own translation unit.
        capsule_helpers = dict(global_capsule_helpers)
        for m in _CAPSULE_HELPER.finditer(src.text):
            capsule_helpers[m.group(1)] = m.group(2) + "Handle"
        capsule_types = dict(global_capsule_types)
        for m in _CONST_FROM_CAST.finditer(src.text):
            capsule_types[m.group(2)] = m.group(1) + "Handle"
        for m in _CONST_FROM_NEW.finditer(src.text):
            capsule_types[m.group(1)] = m.group(2) + "Handle"

        helper_bodies = {m.group(1): body_from(src.text, m.end() - 1)
                         for m in _HELPER.finditer(src.text)}

        # All static helpers in this file, for one-level extraction following:
        # name -> (parameter names, body). Skips prototypes (no body).
        extraction_helpers: dict[str, tuple[list[str], str]] = {}
        for m in _STATIC_HELPER.finditer(src.text):
            params, close = _paren_content(src.text, m.end() - 1)
            brace = src.text.find("{", close)
            semicolon = src.text.find(";", close)
            if brace == -1 or (semicolon != -1 and semicolon < brace):
                continue  # forward declaration, not a definition
            param_names = [_param_name(p) for p in _split_top_level(params)]
            extraction_helpers[m.group(1)] = (param_names, body_from(src.text, brace))

        for m in _FUNCTION.finditer(src.text):
            name = m.group(1)
            if name not in methods:
                continue
            body = body_from(src.text, m.end() - 1)
            fmt, varnames, returns, delegate = _scan_body(body)

            # Functions that just `return helper(args, ...)` inherit the helper's
            # signature and return tokens.
            if fmt is None and delegate in helper_bodies:
                hbody = helper_bodies[delegate]
                hfmt, hvars, hret, _ = _scan_body(hbody)
                fmt, varnames, body = hfmt, hvars, hbody
                returns |= hret
            if fmt is None and methods[name] == "NOARGS":
                fmt = ""
            if fmt is None:
                raise SystemExit(f"could not resolve arguments for {name!r}")

            handle_args = {v: h for v in varnames
                           if (h := _handle_arg(body, v, capsule_types,
                                                extraction_helpers)) is not None}
            # Every capsule argument must resolve to a specific handle type; a
            # silent fallback would let a wrong handle type through the stub
            # unchecked (surfacing only as a runtime TypeError).
            unresolved = [v for v in _capsule_varnames(fmt, varnames)
                          if v not in handle_args]
            if unresolved:
                raise SystemExit(f"{name}: unresolved capsule argument(s) {unresolved}; "
                                 "the generator cannot type them safely")
            functions.append(RawFunction(
                name=name,
                fmt=fmt,
                varnames=tuple(varnames),
                returns=tuple(sorted(returns)),
                handle_args=handle_args,
                return_handle=_return_handle(body, capsule_helpers),
            ))

    missing = set(methods) - {f.name for f in functions}
    if missing:
        raise SystemExit(f"registered but no body found: {sorted(missing)}")
    return functions
