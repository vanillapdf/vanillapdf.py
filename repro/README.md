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

The stdout sink is **spdlog's** default (a stdout/wincolor sink) and spdlog has
declined to change it upstream, so Vanilla.PDF must stop relying on the global
default and drive all logging through its **own logger with a null default
sink**. A library whose consumers legitimately redirect stdout must not emit to
it implicitly.

Do **not** just call `spdlog::set_default_logger(...)`: ~121 call sites use the
free functions `spdlog::{trace,debug,info,warn,error,critical}(...)` (plus the
`LOG_*` macros in `utils/log.h`, which hardcode `spdlog::error`). Those resolve
to the global default logger — the stdout sink — and a raw call that fires
before any lazy init still hits stdout. The guarantee only holds if nothing logs
through the global default.

Concrete plan:

1. **Private logger + lazy init (no load hook).** A function-local accessor,
   initialized once with a null sink; safe before `main`, no `DllMain` (loader
   lock) and no static-init-order fiasco. Return a **snapshot by value**, not a
   `shared_ptr<logger>&`.

   ```cpp
   // returns a stable snapshot; safe default = null sink (never stdout)
   std::shared_ptr<spdlog::logger> vpdf_logger();
   ```

2. **Macro chokepoint + sweep.** Route every call site through macros and ban
   raw spdlog free calls so this can't regress:

   ```cpp
   #define VPDF_LOG_INFO(...)  vpdf_logger()->info(__VA_ARGS__)
   // ... trace/debug/warn/error/critical; redefine LOG_ERROR_AND_THROW* too
   ```

   Migrate all ~121 calls to the macros, then add a CI grep that fails on
   `spdlog::(trace|debug|info|warn|error|critical)\(` under `src/`.

3. **Thread-safe reconfiguration.** `Logging_SetCallbackLogger` /
   `SetRotatingFileLogger` must build a **new** logger and **atomically replace**
   the whole `shared_ptr<logger>` (`std::atomic_store`, or a mutex in the
   accessor) — never mutate `logger->sinks()` in place (that races concurrent
   logging; spdlog locks within a sink, not around sink-vector replacement).
   `SetSeverity` -> `set_level()` is fine directly (level is atomic);
   `SetPattern` mutates the formatter, so route it through the swap or lock.

4. **Migrate the whole `Logging_*` surface** onto the private logger:
   `GetSeverity` -> `level()`, `SetPattern` -> `set_pattern()`,
   `SetRotatingFileLogger`, `SetCallbackLogger`, `Shutdown`.

5. **Release note the behavior change.** Today `Trace` prints to stdout; after
   this the default is silent, so anyone relying on stdout logging must opt into
   a stdout/stderr sink explicitly. (Also: `File constructor`/`File destructor`
   at `info` is very noisy.)

The Python bindings install a `Logging_SetCallbackLogger` sink at import to route
native logs into Python's `logging`, so they don't depend on this native change
landing — but the native null default is still needed to protect unconfigured C
and .NET consumers.

## Python bindings: fixed

The bindings now install a callback logger at import
(`Logging_SetCallbackLogger`) that routes native logs into Python's `logging`
module under the `"vanillapdf"` logger, so spdlog never writes to stdout. This
**fully eliminates** the corruption even under `--capture=fd` (verified: the
Python mirror below now reports `0/300`, and the full test suite is deterministic
with no workaround). `logging_fd_capture_parse_bug.py` therefore reports `0/300`
against a fixed bindings build; the pure-C reproducer above still reproduces
against `libvanillapdf` directly and is the canonical native repro.
