# Reproducers — native logging / fd-capture parse bug

Isolates a bug in the **native Vanilla.PDF C++ library** (not the Python
bindings): with logging enabled and stdout (fd 1) redirected onto a file that is
periodically `ftruncate`'d underneath the library, PDF parsing intermittently
fails with a spurious `VANILLAPDF_ERROR_PARSE_EXCEPTION` ("Could not parse
object at offset N"). This is what pytest's default `--capture=fd` does to its
capture buffer between tests, which is how it first surfaced.

## Confirmed: it is NOT the Python bindings

The pure-C reproducer below hits the bug against `libvanillapdf` directly, with
no Python in the process:

```
logging=TRACE: 11/300 spurious parse failures   (error_type 65540 = PARSE_EXCEPTION)
logging=OFF:    0/300
```

Toggling only the log severity (TRACE↔OFF), everything else identical, flips the
failures on and off. The PDF on disk is byte-valid (only the random trailer
`/ID` differs run-to-run), and Python's debug allocator is clean — so this is an
I/O / logging-state bug inside the library, not malformed output or a bindings
heap issue. Likely area: the logging output path (buffered stdio / stream state,
or a logging-thread race) going inconsistent when fd 1 is truncated out from
under it, bleeding into the parser's I/O.

## `logging_fd_capture_parse_bug.c` — pure C (for the native team)

No Python. Build against the C API and debug under ASan / TSan / gdb.

Linux:

```bash
gcc -g -fsanitize=address repro/logging_fd_capture_parse_bug.c \
    -I <vanillapdf>/include -L <build>/lib -lvanillapdf -o repro && ./repro
./repro --off        # control -> 0/300
```

Or build it from this project's CMake against the already-fetched library:

```bash
cmake -S . -B build -DVANILLAPDF_PY_BUILD_NATIVE_REPRO=ON
cmake --build build --target native_logging_repro --config Release
./build/Release/native_logging_repro        # (.exe on Windows)
```

## `logging_fd_capture_parse_bug.py` — Python (mirror)

Same scenario through the bindings; requires the built `vanillapdf` package.

```bash
python repro/logging_fd_capture_parse_bug.py         # logging on  -> N/300 (N>0)
python repro/logging_fd_capture_parse_bug.py --off   # logging off -> 0/300
```

## Bindings workaround (until the native fix)

`tests/conftest.py` disables native logging per-test so the Python suite is
deterministic. It is a documented **workaround**, not a fix — remove it once the
native library bug is resolved.
