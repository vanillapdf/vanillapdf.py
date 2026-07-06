"""Locating and reading the native C++ sources, and brace-matching bodies.

Keeps all filesystem/text concerns in one place so the parser can work on
in-memory strings.
"""
from __future__ import annotations

import glob
import os
from dataclasses import dataclass

# scripts/stubgen/sources.py -> repo root is two directories up.
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
NATIVE_GLOB = os.path.join(REPO_ROOT, "src", "vanillapdf_native", "**", "*.cpp")
STUB_PATH = os.path.join(REPO_ROOT, "src", "vanillapdf", "_vanillapdf.pyi")


@dataclass(frozen=True)
class Source:
    """One native translation unit."""

    path: str
    text: str


def read_sources() -> list[Source]:
    """Return every native ``.cpp`` source, sorted for deterministic output."""
    paths = sorted(set(glob.glob(NATIVE_GLOB, recursive=True)))
    if not paths:
        raise SystemExit(f"no native sources found under {NATIVE_GLOB}")
    return [Source(p, open(p, encoding="utf-8", errors="replace").read()) for p in paths]


def body_from(text: str, brace_open: int) -> str:
    """Return the ``{...}`` block beginning at ``brace_open`` (brace-matched)."""
    depth = 0
    for i in range(brace_open, len(text)):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return text[brace_open:i + 1]
    return text[brace_open:]
