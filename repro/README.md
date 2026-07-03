# Reproducers — native logging corrupts saved PDFs under stdout redirection

Isolates a bug in the **native Vanilla.PDF C++ library** (not the Python
bindings).

## What actually happens

The library's logging is **enabled by default at INFO with a stdout sink**.
When the application redirects stdout (fd 1) onto a file and juggles it with
`dup2` — exactly what pytest's default `--capture=fd` does to its capture buffer
between tests — the stdout sink's cached OS output handle goes stale/aliased,
and log writes intermittently land **in the wrong file: the very PDF the library
is saving**. The library's own log text (e.g. `[info] File destructor …` from
`File::~File`) gets written into the saved PDF, clobbering the
`>> … startxref … %%EOF` tail of the trailer.

The result is a **genuinely corrupt file on disk**. The parser then correctly
refuses to parse it — so the `PARSE_EXCEPTION` ("Could not parse object at offset
N") is a *real* parse error on a broken file, **not** a parser race or "logging
state bleeding into parser I/O."

## Evidence

| Test | Result |
|------|--------|
| logging TRACE, `--capture=fd` pattern (dup2 fd 1 → file + truncate) | intermittent parse failures (`error_type 65540`) |
| same, logging OFF | 0 |
| re-parse the failing file on disk (no truncation) | still fails at the same offset → file genuinely corrupt |
| redirect stdout via `freopen` (clean fd, no juggling) | 0 |
| route logs through a callback sink instead of stdout, same fd pattern | 0 |

The smoking gun: the corrupt file's trailer contains spdlog lines, e.g.
`… /Root 1 0 R /Size 6[2026-… ] [info] File destructor …`. Single-threaded
throughout — not a thread race.

## `logging_fd_capture_parse_bug.c` — pure C (for the native team)

No Python. Build against the C API and debug under ASan / gdb.

Linux:

```bash
gcc -g -fsanitize=address repro/logging_fd_capture_parse_bug.c \
    -I <vanillapdf>/include -L <build>/lib -lvanillapdf -o repro && ./repro
./repro --off        # control -> 0/300
```

Or from this project's CMake, against the already-fetched library:

```bash
cmake -S . -B build -DVANILLAPDF_PY_BUILD_NATIVE_REPRO=ON
cmake --build build --target native_logging_repro --config Release
./build/Release/native_logging_repro        # (.exe on Windows)
```

## Recommended native fix

Don't default diagnostics to the stdout sink — a library whose consumers
legitimately redirect stdout shouldn't emit to it implicitly. Default to a null
sink with explicit opt-in (callback / file / stderr), or at minimum document
that enabling logging writes to stdout. (`File constructor`/`File destructor` at
`info` level is also very noisy.)

## Python bindings: fixed

The bindings now install a callback logger at import
(`Logging_SetCallbackLogger`) that routes native logs into Python's `logging`
module under the `"vanillapdf"` logger, so spdlog never writes to stdout. This
**fully eliminates** the corruption even under `--capture=fd` (verified: the
Python mirror below now reports `0/300`, and the full test suite is deterministic
with no workaround). `logging_fd_capture_parse_bug.py` therefore reports `0/300`
against a fixed bindings build; the pure-C reproducer above still reproduces
against `libvanillapdf` directly and is the canonical native repro.
