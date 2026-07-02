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
