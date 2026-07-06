"""Reproducer mirror for the native logging/stdout-capture bug (see README.md).

This documents a bug in the native Vanilla.PDF library. The pure-C
`logging_fd_capture_parse_bug.c` is the canonical reproducer; this Python mirror
now reports 0/300 because the bindings FIX the issue at import (see below).

MECHANISM
    The native library logs to STDOUT (fd 1) by default (INFO). When stdout is
    redirected onto a file and juggled with dup2 (as pytest's `--capture=fd`
    does), the stdout sink's stale/aliased handle intermittently writes log text
    into the PDF the library is SAVING, corrupting the trailer. The parser then
    correctly rejects a genuinely-corrupt file -> spurious PARSE_EXCEPTION.

BINDINGS FIX (why this now reports 0/300)
    The bindings install a callback logger at import that routes native logs
    into Python's `logging` ("vanillapdf" logger), so spdlog never touches
    stdout. To observe the original NATIVE bug, use the C reproducer, which
    links libvanillapdf directly with its default stdout sink.

RUN
    python repro/logging_fd_capture_parse_bug.py           # fixed bindings -> 0/300
    python repro/logging_fd_capture_parse_bug.py --off      # control -> 0/300
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
