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
