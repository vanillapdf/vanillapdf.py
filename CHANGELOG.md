# Changelog

All notable changes to this project are documented here. The format is based on
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## Versioning

These bindings **mirror the version of the underlying
[Vanilla.PDF](https://github.com/vanillapdf/vanillapdf) C++ library they wrap**,
matching the convention used by
[vanillapdf.net](https://github.com/vanillapdf/vanillapdf.net). Version `2.3.0`
of this package wraps version `2.3.0` of the native library, so the two are
always readable together. The pinned native tag lives in `CMakeLists.txt`
(`VANILLAPDF_GIT_TAG`).

Binding-only changes — a fix in the Python or extension layer with no
corresponding native release — get a fourth component: `2.3.0.1`, `2.3.0.2`, and
so on. This is a normal PEP 440 release segment and sorts as expected. Note that
PEP 440 post-releases (`2.3.0.post1`) are deliberately *not* used, as the spec
reserves those for metadata corrections rather than bug fixes.

## [Unreleased]

### Native library
- Wraps Vanilla.PDF `v2.3.0` (previously pinned to `v2.3.0-rc.2`) — the final
  release, which adds AES-256 encryption creation and carries several encryption
  correctness fixes (AES-128 no longer silently written as RC4, array-filter
  streams no longer written in the clear) plus further thread-safety work
  (`File::GetByteRange` seek+read locking, a liveness-aware document-registry
  create guard, and a `Document` catalog lazy-initialization race).

### Added
- `TextStringEncoding` / `TextStringEncodingType`, wrapping the native
  text-string encoding utilities (mirrors .NET's `PdfTextStringEncoding`):
  `detect(data: bytes)` reports a text string's encoding from its byte order
  mark (PDFDocEncoding, UTF-16BE or UTF-8), and
  `pdf_doc_encoding_byte_to_unicode(value)` maps a PDFDocEncoding byte to its
  Unicode code point.
- `TextStringEncoding.decode(data: bytes)` decodes a PDF text string using the
  encoding it declares (UTF-16BE / UTF-8 byte order mark, dropped; otherwise
  PDFDocEncoding), the same decoding as .NET's `PdfTextStringUtils.Decode`.
- `StringObject.text`: the string's value decoded with `TextStringEncoding.decode`.
- Python 3.15 support.
- In-memory open and save: `Document.open_bytes(data)` / `open_stream(stream)`
  open a document from `bytes` or a readable binary stream, and `save_bytes()` /
  `save_stream(stream)` (plus the `_incremental` variants) save into memory or a
  writable binary stream. The data is copied, so the source can be closed right
  after opening.
- `IOStrategyType` and an optional `strategy` argument on `Document.open` /
  `create` and `File.open` / `create`, choosing how a file on disk is read and
  written (default `FILE_STREAM`; `MEMORY` loads it whole).
- Paths accept `os.PathLike` (e.g. `pathlib.Path`) as well as `str`.

### Changed
- Closing an object while another thread is still inside a call on it no
  longer frees the native object under that call. Each call holds a
  reference to the handle while it runs (as .NET's `SafeHandle` does);
  `close()` takes effect at once for new calls, and the native release
  happens when the last running call returns.
- Every call into the native library now releases the GIL, including handle
  releases (`close()`, garbage collection), not only the blocking ones. Other
  Python threads keep running during any native work, and native code can later
  call back into Python (caller-supplied streams) without dead-locking.
  `scripts/check_gil_release.py` enforces this in CI.
- **Breaking:** `Document` and `File` are created with class methods mirroring
  the native API: `Document.open(path)` replaces `Document(path)`, and
  `File.open(path)` replaces `File(path)`.
- vcpkg baseline moved from `2025.12.12` to `2026.10.16`, updating the bundled
  C++ dependencies: OpenSSL 3.6.0 → 3.6.5, zlib 1.3.1 → 1.3.2, libjpeg-turbo
  3.1.2 → 3.2.0, spdlog 1.16.0 → 1.17.0, fmt 12.1.0 → 12.2.0. OpenSSL 3.6.4 and
  3.6.5 are security releases (most severe: High); fixes relevant to PDF
  signing and verification include a heap overflow in CMS key unwrapping,
  excessive memory use in relative CRL distribution point processing, and
  timing side-channels in EC scalar multiplication.
- Docs are built with Sphinx 9 (`sphinx>=9.1,<10`).
- **Breaking:** Python 3.11 or later is required; Python 3.10 (end of life in
  October 2026) is no longer supported.
- Wheels are built against the stable ABI: one `cp311-abi3` wheel per platform
  installs on CPython 3.11 and every later version (including future ones),
  replacing the per-version `cp310` … `cp314` wheels. Free-threaded CPython
  (`3.15t`) is not covered by abi3 and has no wheel yet.
- Building from source requires CMake 3.26 or later (previously 3.20) and
  scikit-build-core 1.0 or later; the version provider moved to the standard
  `[[tool.dynamic-metadata]]` table.
- **Breaking:** `DocumentEncryptionSettings.set_owner_password` /
  `set_user_password` take a `str` only (encoded as UTF-8, matching
  `File.set_encryption_password`); `bytes` is no longer accepted.
- **Breaking:** `TrustedCertificateStore.add_certificate_from_pem` and
  `add_certificate_from_der` take `bytes` only (the file contents, e.g.
  `open(path, "rb").read()`). Passing a `str` to the DER variant previously
  UTF-8-encoded it, corrupting any non-ASCII byte.
- **Breaking:** public signatures no longer take or return a `Buffer`; the
  wrappers create and release it internally:
  - `StringObject.value` setter takes `bytes` (matching the getter).
  - `PKCS12Key.create_from_buffer(buffer, password)` is now
    `PKCS12Key.create_from_data(data: bytes, password)`.
  - `LicenseInfo.set_license_buffer(buffer)` is now
    `LicenseInfo.set_license_data(data: bytes)`.
  - `File.get_filename()` returns the filename as a `str`;
    `File.get_filename_string()` is removed.
- **Breaking:** `StringObject.value_string()` is removed; use `.text`. It
  decoded every string as UTF-8, which fails on PDFDocEncoding strings and
  raised on UTF-16BE ones (most non-ASCII document titles).
  `NameObject.value_string()` is unchanged (names are UTF-8).

### Fixed
- `IndirectReferenceObject.object_number`, `File.get_indirect_object` and
  `Errors.get_printable_error_text` raise `OverflowError` for negative or
  too-large values instead of silently wrapping them (`object_number = -1`
  stored `2**64 - 1`). Python 3.15 deprecates the wrapping behaviour.
- Temporary native buffers returned by `Object.to_string` / `to_pdf`,
  `NameObject.value`, `StringObject.value`, `StreamObject.body` / `body_raw` and
  the `SignatureVerificationResult` text fields are now released immediately
  instead of waiting for garbage collection.

## [2.3.0-rc.2]

First release under the mirrored versioning scheme described above; the jump
from `0.0.1` reflects the change in scheme, not a sudden change in the code. The
bindings grew from a thin wrapper into a broadly usable, typed, thread-safe PDF
library. Highlights since `0.0.1`:

### Native library
- Wraps Vanilla.PDF `v2.3.0-rc.2` (previously pinned to `v2.3.0-rc.1`), which
  carries the fix for the object-registry race that made concurrent
  `Document.create` calls flaky.

### Added
- **Syntax object model** — `Object` plus every concrete type (`Null`, `Boolean`,
  `Integer`, `Real`, `Name`, `String`/`LiteralString`/`HexadecimalString`,
  `Array`, `Dictionary`, `Stream`, `IndirectReference`) with Pythonic protocols
  (`len`/indexing/iteration on arrays and dictionaries, `str`-keyed dictionaries).
- **Semantic layer** — `Document` (open/create/save/save-incremental/append),
  `Catalog`, `PageTree`, `PageObject`, `Rectangle`, `DocumentInfo`.
- **Encryption** — `DocumentEncryptionSettings` (algorithm, key length, user
  access permissions, user/owner passwords) with `Document.add_encryption` /
  `remove_encryption`.
- **Signing & verification** — `PKCS12Key`, `SigningKey`,
  `DocumentSignatureSettings`, `Document.sign`; `TrustedCertificateStore`,
  `SignatureVerificationSettings`, `SignatureVerificationResult`, and
  `DigitalSignature.verify`.
- **Interactive forms (read)** — `InteractiveForm`, `FieldCollection`, `Field`,
  `SignatureField`.
- **Utilities** — `Buffer`, `Errors`/`PdfError` (with numeric `error_code`),
  `LibraryInfo`, `LicenseInfo`, `MiscUtils`, `Logging`.
- **Typed distribution** — the whole package is checked under pyright `strict`,
  and ships type information to downstream users (PEP 561 `py.typed` + a generated
  `_vanillapdf.pyi` stub via `scripts/generate_native_stub.py`).
- **Documentation** — a Sphinx / Read the Docs site.
- **`Logging.set_rotating_file()`** — opt-in native log routing to a rotating
  file sink (streams to disk).
- **CI** — pyright gate; a sanitizer workflow (ASan + UBSan) that runs on native
  changes and weekly; malformed-PDF robustness tests; wheels are now installed
  and tested before publishing, and Linux `aarch64` wheels are built.

### Changed
- **True multi-threaded parallelism** — blocking native calls now release the GIL
  (`without_gil`), so threaded callers get real C-level concurrency instead of
  serializing on the GIL.
- Dictionary keys are `str` (the redundant runtime type checks were removed now
  that the API is statically typed).
- Wheels ship only the Python package and the extension — the C++ SDK tree
  (headers, CMake config, tools) is no longer bundled.

### Fixed
- Native logging no longer corrupts saved files: the default stdout sink (which
  could write log text into a PDF under `pytest --capture=fd`) is replaced with a
  discarding sink. Routing to Python `logging` was removed because it dead-locked
  once native calls release the GIL.

## [0.0.1]

- Initial release: a thin wrapper exposing `Document` (open/save), `Buffer`,
  `File`, `LibraryInfo`, and `Logging`.

[Unreleased]: https://github.com/vanillapdf/vanillapdf.py/compare/v2.3.0-rc.2...HEAD
[2.3.0-rc.2]: https://github.com/vanillapdf/vanillapdf.py/compare/v0.0.1...v2.3.0-rc.2
[0.0.1]: https://github.com/vanillapdf/vanillapdf.py/releases/tag/v0.0.1
