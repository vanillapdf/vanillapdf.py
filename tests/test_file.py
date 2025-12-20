from pathlib import Path
import vanillapdf


def test_file_open_and_version():
    base = Path(__file__).parent
    test_input = base.parent / "assets" / "pdf-test.pdf"

    assert test_input.exists(), f"{test_input} not found"

    with vanillapdf.File(str(test_input)) as f:
        f.initialize()
        version = f.version
        assert version in ["1.0", "1.1", "1.2", "1.3", "1.4", "1.5", "1.6", "1.7", "2.0"]


def test_file_get_filename():
    base = Path(__file__).parent
    test_input = base.parent / "assets" / "pdf-test.pdf"

    assert test_input.exists(), f"{test_input} not found"

    with vanillapdf.File(str(test_input)) as f:
        assert f.filename == "pdf-test.pdf"


def test_file_is_encrypted():
    base = Path(__file__).parent
    test_input = base.parent / "assets" / "pdf-test.pdf"

    assert test_input.exists(), f"{test_input} not found"

    with vanillapdf.File(str(test_input)) as f:
        f.initialize()
        # Our test file should not be encrypted
        assert f.is_encrypted is False


def test_file_unicode_filename():
    """Test that Unicode (Chinese) filenames work correctly."""
    base = Path(__file__).parent
    test_input = base.parent / "assets" / "chinese_names_中文的名字-2.pdf"

    assert test_input.exists(), f"{test_input} not found"

    with vanillapdf.File(str(test_input)) as f:
        f.initialize()
        version = f.version
        assert version in ["1.0", "1.1", "1.2", "1.3", "1.4", "1.5", "1.6", "1.7", "2.0"]
        assert f.filename == "chinese_names_中文的名字-2.pdf"


test_file_open_and_version()
test_file_get_filename()
test_file_is_encrypted()
test_file_unicode_filename()
