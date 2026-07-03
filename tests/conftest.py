from pathlib import Path

import pytest

ASSETS_DIR = Path(__file__).parent.parent / "assets"


@pytest.fixture(autouse=True)
def _silence_native_logging():
    """WORKAROUND (not a fix) for a native-library bug: disable native logging.

    The C++ library logs to stdout (fd 1). Under pytest's default `--capture=fd`
    (which dup2's a temp file over fd 1 and truncates it between tests), the
    library's logging I/O state gets corrupted and bleeds into the parser,
    causing spurious `PARSE_EXCEPTION`s. This is isolated, without pytest, in
    `repro/logging_fd_capture_parse_bug.py` and must be fixed in the native
    library. Until then we disable logging per-test (per-test so test_logging
    can't re-enable it for a later test). Remove this once the native bug is
    fixed. Do NOT paper over it by loosening assertions elsewhere.
    """
    from vanillapdf import Logging, LoggingSeverity

    Logging.set_severity(LoggingSeverity.OFF)


@pytest.fixture
def assets_dir() -> Path:
    """Path to the repository's test assets directory."""
    return ASSETS_DIR


@pytest.fixture
def test_pdf() -> str:
    """Path to the standard test PDF as a string."""
    return str(ASSETS_DIR / "pdf-test.pdf")


@pytest.fixture
def signing_key_path() -> str:
    """Path to the bundled PKCS#12 test key (password: 'test')."""
    return str(ASSETS_DIR / "test-signing-key.pfx")


@pytest.fixture
def plain_pdf(tmp_path):
    """Create a fresh, unencrypted single-page PDF and return its path.

    The bundled assets/pdf-test.pdf is itself encrypted (empty user password),
    so tests that need a plaintext source generate one here.
    """
    from vanillapdf import Document, File, PageObject, Rectangle

    path = str(tmp_path / "plain.pdf")
    with Document.create(path) as doc:
        page = PageObject.create_from_document(doc)
        rect = Rectangle.create()
        rect.lower_left_x = 0
        rect.lower_left_y = 0
        rect.upper_right_x = 200
        rect.upper_right_y = 300
        page.media_box = rect
        doc.get_catalog().get_pages().append(page)
        doc.save(path)

    with File(path) as f:
        f.initialize()
        assert f.is_encrypted is False  # sanity: the source really is plaintext
    return path
