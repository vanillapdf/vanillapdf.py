import pytest

import vanillapdf


def test_logging_get_severity():
    severity = vanillapdf.Logging.get_severity()
    assert isinstance(severity, vanillapdf.LoggingSeverity)


def test_logging_set_severity():
    # Save original severity
    original = vanillapdf.Logging.get_severity()

    # Set to DEBUG
    vanillapdf.Logging.set_severity(vanillapdf.LoggingSeverity.DEBUG)
    assert vanillapdf.Logging.get_severity() == vanillapdf.LoggingSeverity.DEBUG

    # Set to WARNING
    vanillapdf.Logging.set_severity(vanillapdf.LoggingSeverity.WARNING)
    assert vanillapdf.Logging.get_severity() == vanillapdf.LoggingSeverity.WARNING

    # Restore original
    vanillapdf.Logging.set_severity(original)


def test_logging_severity_enum():
    assert vanillapdf.LoggingSeverity.TRACE < vanillapdf.LoggingSeverity.DEBUG
    assert vanillapdf.LoggingSeverity.DEBUG < vanillapdf.LoggingSeverity.INFO
    assert vanillapdf.LoggingSeverity.INFO < vanillapdf.LoggingSeverity.WARNING
    assert vanillapdf.LoggingSeverity.WARNING < vanillapdf.LoggingSeverity.ERROR
    assert vanillapdf.LoggingSeverity.ERROR < vanillapdf.LoggingSeverity.CRITICAL
    assert vanillapdf.LoggingSeverity.CRITICAL < vanillapdf.LoggingSeverity.OFF


def test_logging_set_pattern():
    # Just verify it doesn't throw
    vanillapdf.Logging.set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v")


def test_rotating_file_logger(tmp_path):
    """Native logs opt into a rotating file sink and are written to disk."""
    log_path = tmp_path / "native.log"
    original = vanillapdf.Logging.get_severity()
    vanillapdf.Logging.set_rotating_file(str(log_path), 1024 * 1024, 2)
    try:
        vanillapdf.Logging.set_severity(vanillapdf.LoggingSeverity.DEBUG)
        with vanillapdf.Document.create(str(tmp_path / "out.pdf")) as doc:
            doc.get_catalog().get_pages().append(
                vanillapdf.PageObject.create_from_document(doc))
            doc.save(str(tmp_path / "out.pdf"))
    finally:
        vanillapdf.Logging.set_severity(original)
        vanillapdf.Logging.shutdown()  # closes the file, back to the default sink
    assert log_path.stat().st_size > 0


def test_shutdown_keeps_library_usable(tmp_path):
    """Shutdown used to drop the native default logger, so the next call that
    logged (opening a missing file logs an error) crashed the interpreter."""
    vanillapdf.Logging.shutdown()
    with pytest.raises(vanillapdf.PdfError):
        vanillapdf.Document.open(str(tmp_path / "missing.pdf"))


def test_shutdown_releases_rotating_file(tmp_path):
    """Shutdown closes the rotating log file; the sink can then be set again,
    to a new path, and logs reach the new file."""
    first = tmp_path / "first.log"
    second = tmp_path / "second.log"
    vanillapdf.Logging.set_rotating_file(str(first), 1024 * 1024, 2)
    vanillapdf.Logging.shutdown()
    first.unlink()  # fails on Windows while the file is still open

    vanillapdf.Logging.set_rotating_file(str(second), 1024 * 1024, 2)
    try:
        with pytest.raises(vanillapdf.PdfError):
            vanillapdf.Document.open(str(tmp_path / "missing.pdf"))  # logs an error
    finally:
        vanillapdf.Logging.shutdown()
    assert second.stat().st_size > 0
