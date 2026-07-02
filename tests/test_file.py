import pytest
import vanillapdf


def test_file_open_nonexistent_raises_pdf_error():
    """Opening a missing file surfaces the library's real error as PdfError."""
    with pytest.raises(vanillapdf.PdfError) as exc:
        vanillapdf.File("this-file-does-not-exist-Ω.pdf")
    assert isinstance(exc.value.error_code, int)
    assert "File_Open" in str(exc.value)


def test_file_use_after_close_raises(test_pdf):
    f = vanillapdf.File(test_pdf)
    f.close()
    with pytest.raises(ValueError):
        f.get_version()
    # close() is idempotent
    f.close()


def test_file_open_and_version(test_pdf):
    with vanillapdf.File(test_pdf) as f:
        f.initialize()
        version = f.get_version()
        assert isinstance(version, vanillapdf.PDFVersion)
        version_str = f.get_version_string()
        assert version_str in ["1.0", "1.1", "1.2", "1.3", "1.4", "1.5", "1.6", "1.7", "2.0"]


def test_file_get_filename(test_pdf):
    with vanillapdf.File(test_pdf) as f:
        assert f.get_filename_string() == "pdf-test.pdf"


def test_file_is_encrypted(test_pdf):
    with vanillapdf.File(test_pdf) as f:
        f.initialize()
        # Our test file is encrypted
        assert f.is_encrypted is True


def test_file_unicode_filename(assets_dir):
    """Test that Unicode (Chinese) filenames work correctly."""
    test_input = assets_dir / "chinese_names_中文的名字-2.pdf"
    assert test_input.exists(), f"{test_input} not found"

    with vanillapdf.File(str(test_input)) as f:
        f.initialize()
        version = f.get_version()
        assert isinstance(version, vanillapdf.PDFVersion)
        assert f.get_filename_string() == "chinese_names_中文的名字-2.pdf"
