"""Validate a release tag and print the release metadata as GitHub outputs.

Called by the `prepare` job of release.yml; run it locally the same way:

    TAG=v2.3.0.1 python .github/scripts/check_release_tag.py

Environment variables:
    TAG         - Tag to release (e.g. v2.3.0-rc.2). Ignored on pull_request.
    DRY_RUN     - "true" to build and validate without publishing.
    EVENT_NAME  - GitHub event; on "pull_request" the tag is derived from the
                  pinned native version and the run is always a dry run.

Prints `key=value` lines (tag, dry_run, version, prerelease, is_latest) to
stdout for $GITHUB_OUTPUT. Exits with an ::error:: message if the tag is
invalid, already exists, or does not match the pinned native version.
"""

from __future__ import annotations

import os
import re
import subprocess
import sys
from pathlib import Path
from typing import NoReturn

from packaging.version import InvalidVersion, Version


def fail(message: str) -> NoReturn:
    raise SystemExit(f"::error::{message}")


def read_pin() -> str:
    """The native tag these bindings wrap: VANILLAPDF_GIT_TAG in CMakeLists.txt."""
    cmake = Path("CMakeLists.txt").read_text(encoding="utf-8")
    match = re.search(r'^set\(VANILLAPDF_GIT_TAG "([^"]+)"', cmake, re.MULTILINE)
    if match is None:
        fail("Could not read VANILLAPDF_GIT_TAG from CMakeLists.txt")
    return match.group(1)


def existing_tags() -> list[str]:
    result = subprocess.run(["git", "tag", "-l", "v*"], capture_output=True, text=True, check=True)
    return result.stdout.split()


def parse_version(tag: str) -> Version:
    """The PEP 440 version setuptools-scm would derive (v2.3.0-rc.2 -> 2.3.0rc2)."""
    if not tag.startswith("v"):
        fail(f"Tag must start with 'v': {tag}")
    try:
        return Version(tag[1:])
    except InvalidVersion as exc:
        fail(f"Tag is not a valid PEP 440 version: {exc}")


def check_matches_pin(tag: str, pin: str) -> None:
    """These bindings mirror the native version: the tag must equal the pin
    (v2.3.0, v2.3.0-rc.2), or - for a binding-only release of a stable pin -
    add a fourth component (v2.3.0.1)."""
    if tag == pin:
        return
    stable_pin = re.fullmatch(r"v\d+\.\d+\.\d+", pin) is not None
    if stable_pin and re.fullmatch(re.escape(pin) + r"\.\d+", tag):
        return
    fail(
        f"Tag {tag} does not match the pinned native version {pin} "
        f"(expected {pin}, or {pin}.N for a binding-only release of a stable pin)."
    )


def is_latest(version: Version, tags: list[str]) -> bool:
    """A prerelease is never latest; a stable release is latest only if it
    outranks every existing *stable* tag (a higher prerelease tag must not steal
    "latest" from it)."""
    if version.is_prerelease:
        return False
    stable: list[Version] = []
    for tag in tags:
        try:
            candidate = Version(tag[1:])
        except InvalidVersion:
            continue
        if not candidate.is_prerelease and not candidate.is_devrelease:
            stable.append(candidate)
    return not stable or version >= max(stable)


def main() -> None:
    pin = read_pin()
    tags = existing_tags()

    if os.environ.get("EVENT_NAME") == "pull_request":
        # Validate the release workflow itself: release the pinned native version,
        # never publish. It may already be released, so the tag is not checked.
        tag, dry_run = pin, "true"
    else:
        tag, dry_run = os.environ["TAG"], os.environ["DRY_RUN"]
        # Publishing the draft release is what creates the tag. If it already
        # existed, the release would bind to an untagged-* placeholder instead.
        if tag in tags:
            fail(f"Tag {tag} already exists. Pick a new version.")

    version = parse_version(tag)
    check_matches_pin(tag, pin)

    outputs = {
        "tag": tag,
        "dry_run": dry_run,
        "version": str(version),
        "prerelease": str(version.is_prerelease).lower(),
        "is_latest": str(is_latest(version, tags)).lower(),
    }
    for key, value in outputs.items():
        print(f"{key}={value}")
        print(f"{key:>10}: {value}", file=sys.stderr)


if __name__ == "__main__":
    main()
