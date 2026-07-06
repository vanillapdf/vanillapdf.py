import subprocess
import sys

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
    """Native logs opt into a rotating file sink and are written to disk. Run in
    a subprocess so replacing the global sink doesn't leak into other tests."""
    log_path = tmp_path / "native.log"
    out_pdf = tmp_path / "out.pdf"
    code = "\n".join([
        "from vanillapdf import Logging, LoggingSeverity, Document, PageObject, Rectangle",
        f"Logging.set_rotating_file({str(log_path)!r}, 1048576, 2)",
        "Logging.set_severity(LoggingSeverity.DEBUG)",
        f"doc = Document.create({str(out_pdf)!r})",
        "page = PageObject.create_from_document(doc)",
        "rect = Rectangle.create()",
        "rect.upper_right_x = 200",
        "rect.upper_right_y = 300",
        "page.media_box = rect",
        "doc.get_catalog().get_pages().append(page)",
        f"doc.save({str(out_pdf)!r})",
        "doc.close()",
        "Logging.shutdown()",
    ])
    subprocess.run([sys.executable, "-c", code], check=True, capture_output=True)
    assert log_path.exists()
    assert log_path.stat().st_size > 0
