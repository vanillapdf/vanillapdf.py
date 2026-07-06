# Changelog

All notable changes to this project are documented here. The format is based on
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and the project aims to
follow [Semantic Versioning](https://semver.org/spec/v2.0.0.html) for its public
API (the names exported from the top-level `vanillapdf` package) once it reaches
`1.0.0`. While in `0.x`, minor versions may include breaking changes.

## [Unreleased]

The bindings grew from a thin wrapper into a broadly usable, typed, thread-safe
PDF library. Highlights since `0.0.1`:

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

[Unreleased]: https://github.com/vanillapdf/vanillapdf.py/compare/v0.0.1...HEAD
[0.0.1]: https://github.com/vanillapdf/vanillapdf.py/releases/tag/v0.0.1
