import pytest

import vanillapdf


def test_open_save_release(test_pdf, tmp_path):
    output = tmp_path / "saved.pdf"
    with vanillapdf.Document(test_pdf) as doc:
        assert doc is not None
        doc.save(str(output))
        assert output.exists()


def test_use_after_close_raises(test_pdf):
    doc = vanillapdf.Document(test_pdf)
    doc.close()
    with pytest.raises(ValueError):
        doc.get_catalog()
    doc.close()  # idempotent
