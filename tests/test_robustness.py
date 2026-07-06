"""Malformed-input robustness: corrupt / truncated / non-PDF data must surface a
`PdfError` (or other Python exception), never crash the interpreter.

A native crash on bad input would take pytest down, so every test that *passes*
is itself evidence the failure was handled gracefully at the boundary.
"""
import pytest

from vanillapdf import Document, File, PdfError


def _write(tmp_path, name: str, data: bytes) -> str:
    path = tmp_path / name
    path.write_bytes(data)
    return str(path)


def _force_parse(path: str) -> None:
    """Open a document and touch its structure so lazy parsing actually runs."""
    with Document(path) as doc:
        len(doc.get_catalog().get_pages())


def test_open_empty_file_raises(tmp_path):
    path = _write(tmp_path, "empty.pdf", b"")
    with pytest.raises(PdfError):
        _force_parse(path)


def test_open_non_pdf_bytes_raises(tmp_path):
    path = _write(tmp_path, "garbage.pdf", b"definitely not a pdf\n" * 32)
    with pytest.raises(PdfError):
        _force_parse(path)


def test_open_header_only_raises(tmp_path):
    # A valid signature but no body, xref, or trailer.
    path = _write(tmp_path, "header_only.pdf", b"%PDF-1.7\n")
    with pytest.raises(PdfError):
        _force_parse(path)


def test_truncated_pdf_raises(tmp_path, test_pdf):
    with open(test_pdf, "rb") as fh:
        data = fh.read()
    # Keep the header, drop the tail (xref/trailer/startxref live at the end).
    path = _write(tmp_path, "truncated.pdf", data[: len(data) // 2])
    with pytest.raises(PdfError):
        _force_parse(path)


def test_corrupted_tail_raises(tmp_path, test_pdf):
    with open(test_pdf, "rb") as fh:
        data = bytearray(fh.read())
    # Zero out the last 256 bytes, wrecking startxref / trailer.
    for i in range(1, min(256, len(data)) + 1):
        data[-i] = 0
    path = _write(tmp_path, "corrupt_tail.pdf", bytes(data))
    with pytest.raises(PdfError):
        _force_parse(path)


def test_wrong_password_raises(tmp_path, test_pdf):
    """The bundled PDF is encrypted (empty user password); a wrong password must
    fail cleanly rather than crash."""
    with File(test_pdf) as handle:
        handle.initialize()
        if not handle.is_encrypted:
            pytest.skip("test asset is not encrypted")
        with pytest.raises(PdfError):
            handle.set_encryption_password("definitely-the-wrong-password")
            # Force a decrypt by resolving an object.
            handle.get_indirect_object(1, 0)
