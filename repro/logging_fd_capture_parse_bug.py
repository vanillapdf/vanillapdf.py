"""Minimal reproducer: native logging + fd-level stdout capture -> spurious parse errors.

This isolates a bug in the native Vanilla.PDF library (NOT the Python bindings)
for debugging in the C++ codebase. No pytest required.

SYMPTOM
    Intermittent, spurious `VANILLAPDF_ERROR_PARSE_EXCEPTION
    ("Could not parse object at offset N")` while parsing well-formed PDFs.

CONDITIONS (all required; remove any one and it never fails)
    1. Native logging enabled (spdlog). The library logs to STDOUT (fd 1).
    2. fd 1 is redirected via dup2() onto a regular file.
    3. That file is periodically ftruncate(0)+lseek(0)'d underneath the library
       while it keeps writing log lines to it (fd shares the file offset via the
       dup2'd description).
    This is exactly what pytest's default `--capture=fd` does to its capture
    buffer between tests, which is how this first surfaced (the Python test
    suite; see tests/conftest.py, which disables logging as a workaround).

EVIDENCE that it is not the file or the Python layer
    - The generated PDF is byte-valid and deterministic apart from the random
      trailer /ID; the failing offset is a fixed, valid object.
    - Python's debug allocator (PYTHONMALLOC=debug) reports nothing -> not a
      PyMem/bindings heap issue.
    - Toggling ONLY the log severity (TRACE vs OFF) flips failures on/off with
      everything else identical (see the control run below).

LIKELY AREA
    The library's logging output path (buffered stdio / stream state, or a
    logging-thread race) becomes inconsistent when the underlying fd-1 file is
    truncated/seeked out from under it, and that corruption bleeds into the
    parser's I/O. A native debugger / AddressSanitizer / ThreadSanitizer on this
    loop should pinpoint it.

RUN
    python repro/logging_fd_capture_parse_bug.py           # expect: N/300 failures (N>0)
    python repro/logging_fd_capture_parse_bug.py --off      # control: 0/300 failures
"""
import os
import sys
import tempfile

from vanillapdf import (
    Document,
    File,
    Logging,
    LoggingSeverity,
    PageObject,
    Rectangle,
)

ITERATIONS = 300
LOGGING_OFF = "--off" in sys.argv


def make_pdf(path):
    with Document.create(path) as doc:
        page = PageObject.create_from_document(doc)
        rect = Rectangle.create()
        rect.lower_left_x = 0
        rect.lower_left_y = 0
        rect.upper_right_x = 200
        rect.upper_right_y = 300
        page.media_box = rect
        doc.get_catalog().get_pages().append(page)
        doc.save(path)


def deep_parse(path):
    # Parse every object -> plenty of log output per iteration.
    with File(path) as f:
        f.initialize()
        for num in range(1, 6):
            try:
                with f.get_indirect_object(num, 0) as obj:
                    obj.to_pdf()
            except Exception:
                pass


def main():
    Logging.set_severity(LoggingSeverity.OFF if LOGGING_OFF else LoggingSeverity.TRACE)

    workdir = tempfile.mkdtemp(prefix="vpdf_repro_")
    pdf = os.path.join(workdir, "doc.pdf")
    cap = os.path.join(workdir, "capture.log")

    saved_stdout = os.dup(1)
    cap_fd = os.open(cap, os.O_RDWR | os.O_CREAT | os.O_TRUNC)
    os.dup2(cap_fd, 1)  # fd 1 -> capture file (mimics pytest --capture=fd)

    fails = 0
    try:
        for i in range(ITERATIONS):
            try:
                make_pdf(pdf)
                deep_parse(pdf)
            except Exception as exc:
                fails += 1
                os.dup2(saved_stdout, 1)
                if fails <= 3:
                    sys.stderr.write(f"iter {i}: {exc}\n")
                os.dup2(cap_fd, 1)
            # mimic pytest reading + truncating the capture buffer between tests
            os.lseek(cap_fd, 0, os.SEEK_SET)
            os.read(cap_fd, 1 << 16)
            os.lseek(cap_fd, 0, os.SEEK_SET)
            os.ftruncate(cap_fd, 0)
    finally:
        os.dup2(saved_stdout, 1)
        os.close(saved_stdout)
        os.close(cap_fd)

    mode = "OFF" if LOGGING_OFF else "TRACE"
    sys.stderr.write(f"logging={mode}: {fails}/{ITERATIONS} spurious parse failures\n")
    return 1 if (fails and LOGGING_OFF) else 0


if __name__ == "__main__":
    raise SystemExit(main())
