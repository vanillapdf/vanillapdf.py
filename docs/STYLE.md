# Coding style

Conventions for this repository. The native extension is **C++** and calls the
VanillaPDF **C API** only.

## Native extension (C++)

### Layout
- Sources mirror the upstream C API: one file per type under `utils/`,
  `syntax/`, `semantics/` (no `…module` suffix). `common.*` and
  `vanillapdfmodule.cpp` live at the native root.
- Includes resolve from the native source root, e.g.
  `#include "syntax/object.h"` (configured via `target_include_directories`).
- Register every new source file in `CMakeLists.txt` and its methods in
  `vanillapdfmodule.cpp`.

### Pointers, casts, conversions
- Use `nullptr`, never `NULL`.
- **No C-style casts, and no casts nested inline in an expression.** Prefer a
  conversion helper; if an explicit cast is genuinely needed, put it on its own
  line assigned to a named variable (e.g. `ObjectType type = static_cast<...>(v);`).
- Native → Python: use the overloaded `to_python(value)` (in `conversion.h`) —
  no `PyLong_From*` cast at the call site.
- Range-checked narrowing: use `safe_convert<To>(from, &out)` (in
  `conversion.h`, signedness-aware, sets `OverflowError`) instead of a narrowing
  cast. `size_type` → `size_t` is guarded by the `static_assert` in `common.h`.
- Same-width conversions (e.g. `long long` → `bigint_type`) need no cast — pass
  the value directly and let the implicit conversion happen.
- Don't write redundant casts. `string_type` is `const char*`, so don't cast a
  `const char*` to it. For `void*` returns, prefer `capsule_cast<T>` /
  `py_malloc<T>`; a bare `static_cast` on its own line is fine where those don't apply.

### Error handling
- Every function returns `error_type`. Give **each call its own `error_type`
  variable** — do not reuse one across calls, so a later check can't read an
  earlier result. Name them for the call (`create_err`, `to_object_err`, …).
- Check immediately and, on failure, `return raise_last_error(err, "C_Api_Name");`
  — never a generic `PyErr_SetString(PyExc_RuntimeError, "Failed to …")`.
- `raise_last_error` runs inline right after the failing call (same thread, GIL
  held) so the library's thread-local error state is intact.
- Map missing optional entries to Python `None` via `is_missing_error(err)`.

### Handles and memory
- Wrap native handles with the `common.h` helpers — no hand-rolled box structs:
  `capsule_new(handle, "VanillaPDF.Type", RELEASE_FN(Type_Release))`,
  `capsule_cast<T>(capsule, "VanillaPDF.Type")`, `capsule_release(...)`.
- Allocate with `py_malloc<T>()`, not a cast around `PyMem_Malloc`.
- Free/release on error paths with a scope guard. Use `SCOPE_GUARD([h]{
  X_Release(h); })` for the common fire-and-forget case; use a named
  `auto g = make_scope_guard([h]{ … })` only when you need `g.dismiss()`
  (ownership transfers out, e.g. to a capsule or the caller). **Capture the
  specific variables explicitly (`[h]`, `[data]`), never `[&]`.**
- Every out-parameter handle is AddRef'd by the library → exactly one matching
  `X_Release` per handle.

### Syntax objects
- Every syntax object is exposed to Python as a single `"VanillaPDF.Object"`
  capsule. Constructors build the concrete typed handle, then
  `X_ToObject` to the base `ObjectHandle` (releasing the temp with a guard).
- Typed accessors convert base → concrete with
  `object_cast<T>(capsule, X_FromObject, "X_FromObject")` (in `syntax/object.h`)
  and release the temp handle with a scope guard.

## Python (`src/vanillapdf/`)
- snake_case modules exporting PascalCase classes; re-export public names from
  the top-level `__init__.py` (`__all__`) so the public API is flat
  (`vanillapdf.IntegerObject`) regardless of internal location.
- Modules are grouped into `syntax/`, `utils/`, `semantics/` subpackages
  mirroring the C++ layers; `handle.py` (shared base) stays at the package root.
  Unlike the C++ side, related classes stay **grouped** per module (e.g. all
  object types in `syntax/objects.py`) — Python idiom favors grouping, and it
  keeps `Object._wrap` dispatch free of circular imports.
- Handle-owning classes extend `Handle` (`handle.py`): set
  `_release = staticmethod(_vanillapdf.x_release)`, assign `self._handle` in
  `__init__`, and use `self._require_handle()` before native calls. `Handle`
  provides context-manager support, idempotent `close()`, and a `__del__` safe
  against a failed `__init__`.
- Pull the handle into a local before a native call rather than nesting
  `self._require_handle()` as an argument.
- Manage handle lifetimes with `with`, not nested `try/finally`. When several
  handles are live at once, chain them in one parenthesized `with` (Python
  3.10+); later items may reference earlier ones, and they close in reverse
  order on exit. Prefer:

  ```python
  with (
      PKCS12Key.create_from_file(path, pw) as key,
      key.to_signing_key() as signing_key,
      DocumentSignatureSettings.create() as settings,
      Document(source) as doc,
      File.create(destination) as dest,
  ):
      settings.set_signing_key(signing_key)
      doc.sign(dest, settings)
  ```

  over a `try/finally` pyramid. Reach for `try/finally` (or
  `contextlib.ExitStack`) only when a handle's lifetime can't follow lexical
  scope.
- Enums (`IntEnum`) mirror the native enum values exactly. Do **not** hardcode
  the `VANILLAPDF_ERROR_*` codes — they are link-time constants, not stable
  numbers.
