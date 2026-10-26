#!/usr/bin/env python
"""Check that every call into the native library releases the GIL.

Native code may take library locks (and, with caller-supplied streams, call
back into Python), so a native call made while holding the GIL can deadlock
against a thread that holds such a lock and waits for the GIL. Every direct
call to a vanillapdf C API function (``Xxx_Yyy(...)``) must therefore sit inside
a ``without_gil(...)`` call::

    python scripts/check_gil_release.py    # exit 1 and list offenders

Calls through function pointers count too: a name declared as
``(CALLING_CONVENTION *name)`` or ``release_fn name`` holds a native function,
so ``name(...)`` (or ``box->name(...)``) must also sit inside ``without_gil``.
"""
from __future__ import annotations

import re
from pathlib import Path

NATIVE_DIR = Path(__file__).resolve().parent.parent / "src" / "vanillapdf_native"

# Comments and string/char literals, replaced by spaces (newlines kept) so that
# operation names like "Document_Open" and prose in comments are not matched,
# and line numbers stay correct.
_NOISE = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', re.S)

# A C API call: CamelCase prefix, underscore, CamelCase name, then "(".
# Excluded: the CPython API (Py*, _Py*), which has the same shape, and
# all-caps macros (SCOPE_GUARD, RELEASE_FN).
_NATIVE_CALL = re.compile(
    r"\b(?!Py|_Py)(?![A-Z0-9_]+\b)([A-Z][A-Za-z0-9]*_[A-Z][A-Za-z0-9_]*)\s*\(")

_WITHOUT_GIL = re.compile(r"\bwithout_gil\s*\(")

# Forbidden inside a without_gil(...) call: the Python C-API, and releasing the
# GIL again (the definitions in common.h are not calls, so they never match).
_INSIDE_FORBIDDEN = re.compile(r"\b(_?Py[A-Za-z]*_\w+|(?:release_)?without_gil)\s*\(")

# Function-pointer declarations; the declared names are native functions.
_POINTER_DECL = re.compile(r"\(\s*CALLING_CONVENTION\s*\*\s*(\w+)\s*\)|\brelease_fn\s+(\w+)")


def _blank(match: re.Match[str]) -> str:
    return re.sub(r"[^\n]", " ", match.group(0))


def _released_spans(code: str) -> list[tuple[int, int]]:
    """The argument span of every ``without_gil(...)`` call."""
    spans: list[tuple[int, int]] = []
    for match in _WITHOUT_GIL.finditer(code):
        depth = 0
        for index in range(match.end() - 1, len(code)):
            if code[index] == "(":
                depth += 1
            elif code[index] == ")":
                depth -= 1
                if depth == 0:
                    spans.append((match.end(), index))
                    break
    return spans


def find_offenders(path: Path) -> list[str]:
    code = _NOISE.sub(_blank, path.read_text(encoding="utf-8"))
    spans = _released_spans(code)
    calls = list(_NATIVE_CALL.finditer(code))
    pointers = sorted({a or b for a, b in _POINTER_DECL.findall(code)} - {"release_fn"})
    if pointers:
        calls += re.finditer(rf"(?<![\w.])({'|'.join(pointers)})\s*\(", code)

    relative = path.relative_to(NATIVE_DIR.parent.parent).as_posix()

    def where(position: int) -> str:
        return f"{relative}:{code.count(chr(10), 0, position) + 1}"

    offenders: list[str] = []
    for match in sorted(calls, key=lambda m: m.start()):
        if not any(start <= match.start() < end for start, end in spans):
            offenders.append(f"{where(match.start())}: {match.group(1)} holds the GIL")

    # The reverse mistake crashes instead of dead-locking: without the GIL,
    # nothing may touch the Python C-API or release the GIL a second time.
    for start, end in spans:
        for match in _INSIDE_FORBIDDEN.finditer(code, start, end):
            offenders.append(f"{where(match.start())}: {match.group(1)} runs without the GIL")
    return offenders


def main() -> int:
    sources = sorted([*NATIVE_DIR.rglob("*.cpp"), *NATIVE_DIR.rglob("*.h")])
    offenders = [o for path in sources for o in find_offenders(path)]
    for offender in offenders:
        print(offender)
    if offenders:
        print(f"{len(offenders)} problem(s): wrap every native call in without_gil(...), "
          "and keep the Python C-API out of it.")
        return 1
    print("Every native call releases the GIL, and none of them touches Python.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
