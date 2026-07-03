from pathlib import Path
import pytest

ASSETS_DIR = Path(__file__).parent.parent / "assets"


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
    import vanillapdf
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
