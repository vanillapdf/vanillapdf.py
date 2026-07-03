import pytest

import vanillapdf


def test_pdf_error_is_exception():
    assert issubclass(vanillapdf.PdfError, Exception)


def test_pdf_error_raised_with_error_code():
    """Requesting the printable text of an invalid error code fails in the
    library and must surface as a PdfError carrying a numeric error_code."""
    with pytest.raises(vanillapdf.PdfError) as exc:
        # 999 is not a valid VANILLAPDF_ERROR_* code.
        vanillapdf.Errors.get_printable_error_text(999)

    assert hasattr(exc.value, "error_code")
    assert isinstance(exc.value.error_code, int)
    # The exception message should carry the library's printable text.
    assert str(exc.value)


def test_errors_thread_local_helpers():
    """The thread-local last-error helpers return well-typed values."""
    code = vanillapdf.Errors.get_last_error()
    assert isinstance(code, int)

    # A valid, known code round-trips to a non-empty printable name.
    text = vanillapdf.Errors.get_printable_error_text(code)
    assert isinstance(text, str)
    assert len(text) > 0

    message = vanillapdf.Errors.get_last_error_message()
    assert isinstance(message, str)
