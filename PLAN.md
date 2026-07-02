# API Expansion Plan — VanillaPDF Python Bindings

Goal: wrap enough of the VanillaPDF C API (v2.3.0-beta.1) that the Python
package becomes practically usable — reading and traversing PDF structure,
manipulating syntax objects, and working with pages/metadata — while laying
infrastructure that scales to the remaining ~60 headers.

## Verified C API facts (from upstream source)

These drive the binding design:

1. **Ownership**: every handle returned through an out-parameter is
   `AddRef`'d by the library — including `X_ToObject` / `X_FromObject`
   conversions (see `SafeObjectConvert` in `c_helper.h`) and getters like
   `Document_GetCatalog` or `DictionaryObject_Find`. Each returned handle
   therefore needs exactly one matching `X_Release`.
2. **Errors**: all functions return `error_type`; on failure the code and a
   detail message are stored in thread-local storage, retrievable via
   `Errors_GetLastError` / `Errors_GetLastErrorMessage`, with printable
   names from `Errors_GetPrintableErrorText`.
3. **Missing optional entries** are signaled with
   `VANILLAPDF_ERROR_OBJECT_MISSING` / `VANILLAPDF_ERROR_OPTIONAL_ENTRY_MISSING`
   (e.g. `DocumentInfo_GetTitle` on a PDF without a title). The bindings map
   these to `None` instead of raising.
4. Error constants are `extern const` link-time symbols, not macros — they
   can only be compared in C, not baked into Python.

## Naming conventions (agreed)

- Internal C helpers are **unprefixed**: `HandleBox`, `capsule_new`,
  `capsule_get`, `capsule_release`, `raise_last_error`, `release_fn`.
  No `vpdf_` / `VanillaPDF` prefixes — these symbols are internal to the
  extension module.
- Python-visible exception: **`vanillapdf.PdfError`** (C global `PdfError`,
  exposed as `_vanillapdf.PdfError`), carrying the numeric library error
  code in its `error_code` attribute.
- Capsule names stay in the existing `"VanillaPDF.<Type>"` style
  (e.g. `"VanillaPDF.IntegerObject"`).
- Native files follow the existing `<name>module.c/h` pattern; Python files
  are snake_case modules exporting PascalCase classes.

## Phase 1 — Shared infrastructure + static functions

New files in `src/vanillapdf_native/`:

- **`common.c/h`** — the boilerplate killer:
  - `HandleBox { void* handle; release_fn release; }` + single shared
    capsule destructor (replaces per-type box structs/destructors).
  - `capsule_new(handle, name, release)` — wraps a handle, takes ownership.
  - `capsule_get(capsule, name)` — validated handle extraction.
  - `capsule_release(capsule, name)` — idempotent explicit release.
  - `raise_last_error(err, operation)` — raises `PdfError` with the
    library's printable error text + thread-local detail message.
  - `is_missing_error(err)` — optional-entry detection.
  - `register_exceptions(module)` — creates `PdfError` at module init.
- **`errorsmodule.c/h`** — `errors_get_last_error`,
  `errors_get_printable_error_text`, `errors_get_last_error_message`.
- **`licenseinfomodule.c/h`** — `license_info_set_license_file`,
  `license_info_set_license_buffer`, `license_info_is_valid`,
  `license_info_is_temporary`.
- **`miscutilsmodule.c/h`** — `misc_utils_initialize_openssl`,
  `misc_utils_cleanup_openssl`.

## Phase 2 — Refactor existing modules onto common infra

Rewrite `buffermodule`, `documentmodule`, `filemodule`, `libraryinfomodule`,
`loggingmodule` to use `capsule_*` helpers and `raise_last_error` (real
error messages instead of generic "Failed to ..." strings). Public helper
`buffer_capsule_from_handle` is preserved for cross-module use.

## Phase 3 — Syntax object layer (C)

- **`objectmodule.c/h`** — base `Object` API: `object_get_object_type`,
  `object_type_name` (static), `object_get_offset`, `object_to_string`,
  `object_to_pdf`, `object_release`.
- **`scalarobjectsmodule.c/h`** — scalar types:
  - Null: `create`, `to_object`, `from_object`, `release`
  - Boolean: + `get_value`, `set_value`
  - Integer: + `create_from_integer_value`, `get_integer_value`,
    `set_integer_value` (signed 64-bit covers PDF integers)
  - Real: + `create_from_data(value, precision)`, `get_value`, `set_value`
  - Name: `create_from_decoded_string`, `get_value` (→ Buffer),
    `to_object`, `from_object`, `release`
  - String base: `get_string_type`, `get_value`, `set_value`,
    `to_object`, `from_object`, `release`
  - LiteralString / HexadecimalString: `create_from_decoded_string`,
    `get_value`, `to_string_object`, `from_string_object`, `release`
- **`containerobjectsmodule.c/h`** — container types:
  - Array: `create`, `get_size`, `get_value`, `set_value`, `append`,
    `insert`, `remove`, `clear`, `to_object`, `from_object`, `release`
  - Dictionary: `create`, `get_size`, `find`, `try_find`, `contains`,
    `get_iterator`, `remove`, `clear`, `insert`, `to_object`,
    `from_object`, `release`
  - DictionaryIterator: `get_key`, `get_value`, `is_valid`, `next`, `release`
  - Stream: `create`, `get_header`, `get_body`, `get_body_raw`,
    `set_body`, `to_object`, `from_object`, `release`
  - IndirectReference: `create`, `get_referenced_object_number`,
    `get_referenced_generation_number`, `get_referenced_object`,
    `set_object_number`, `set_generation_number`, `set_referenced_object`,
    `to_object`, `from_object`, `release`
- **`filemodule` addition** — `file_get_indirect_object(obj_num, gen_num)`.

Deferred: xref chain iteration, file writer, object attributes, streams IO.

## Phase 4 — Semantic layer (C)

- **`documentmodule` additions** — `document_create`, `document_open_file`,
  `document_save_incremental`, `document_get_catalog`,
  `document_get_document_info`, `document_append_document`.
- **`catalogmodule.c/h`** — `catalog_get_pages`, `catalog_release`.
- **`pagetreemodule.c/h`** — `page_tree_get_page_count`,
  `page_tree_get_page`, `page_tree_insert_page`, `page_tree_append_page`,
  `page_tree_remove_page`, `page_tree_release`.
- **`pageobjectmodule.c/h`** — `page_object_create_from_document`,
  `page_object_get_media_box`, `page_object_set_media_box`,
  `page_object_get_base_object`, `page_object_release`.
- **`rectanglemodule.c/h`** — `rectangle_create`, 8 corner getters/setters,
  `rectangle_release`.
- **`documentinfomodule.c/h`** — `document_info_get_title` / `author` /
  `subject` / `keywords` / `creator` / `producer` (each returns a
  StringObject or None when absent), `document_info_release`.

Deferred: signing, encryption settings, annotations, outlines, fonts,
content instruction parsing, dates/trapped.

## Phase 5 — Python API

New modules in `src/vanillapdf/`:

- `errors.py` — re-export `PdfError`, `Errors` static helpers.
- `license_info.py` — `LicenseInfo` static class.
- `misc_utils.py` — `MiscUtils.initialize_openssl()` / `cleanup_openssl()`.
- `objects.py` — `ObjectType` enum; `Object` base class with
  `object_type`, `to_string()`, `to_pdf()`, offset; typed subclasses
  `NullObject`, `BooleanObject`, `IntegerObject`, `RealObject`,
  `NameObject`, `StringObject` (+ `LiteralStringObject`,
  `HexadecimalStringObject`), `ArrayObject`, `DictionaryObject`,
  `StreamObject`, `IndirectReferenceObject`; factory `Object._wrap(handle)`
  dispatching on `object_get_object_type` so lookups return the right
  subclass. Pythonic protocols: `__len__`/`__getitem__`/`__iter__` on
  Array and Dictionary, values as native Python types.
- `rectangle.py`, `document_info.py`, `catalog.py`, `page_tree.py`
  (`__len__`, indexing → `PageObject`), `page_object.py`.
- `document.py` extensions: `create()`, `save_incremental()`,
  `get_catalog()`, `get_document_info()`, `append_document()`.
- `file.py` extension: `get_indirect_object()`.
- `__init__.py` re-exports everything public.

All handle-owning classes keep the existing conventions: context manager
support, explicit `close()`, `__del__` fallback, `ValueError` on use after
close.

## Phase 6 — Build, tests, CI

- Register all new sources in `CMakeLists.txt`.
- Tests: `tests/test_errors.py`, `tests/test_objects.py`,
  `tests/test_semantics.py` (catalog → page tree → page → media box;
  document info on `assets/pdf-test.pdf`), extend existing test files.
- Rebuild via `pip install -e .` (vcpkg + vanillapdf compile from scratch),
  then `pytest tests/ -v`.

## Later phases (not in this pass)

- utils: streams (input/output), encryption keys, signing keys, PKCS#12,
  signature verification, buffer array.
- syntax: xref chain/entries, file writer + observer, object attributes,
  file structure validator, filters.
- semantics: annotations, outlines, destinations, interactive forms,
  fonts/character maps, content stream instruction layer, digital
  signatures, page labels, viewer preferences.
