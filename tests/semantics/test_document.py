import io
from pathlib import Path

import pytest

import vanillapdf


def test_open_save_release(test_pdf, tmp_path):
    output = tmp_path / "saved.pdf"
    with vanillapdf.Document.open(test_pdf) as doc:
        assert doc is not None
        doc.save(str(output))
        assert output.exists()


def test_use_after_close_raises(test_pdf):
    doc = vanillapdf.Document.open(test_pdf)
    doc.close()
    with pytest.raises(ValueError):
        doc.get_catalog()
    doc.close()  # idempotent


def test_open_accepts_path_like(test_pdf):
    with vanillapdf.Document.open(Path(test_pdf)) as doc:
        assert doc.get_catalog() is not None


def page_count(doc):
    return len(doc.get_catalog().get_pages())


def test_open_bytes_and_save_bytes_round_trip(plain_pdf):
    with vanillapdf.Document.open(plain_pdf) as doc:
        expected = page_count(doc)

    with vanillapdf.Document.open_bytes(Path(plain_pdf).read_bytes()) as doc:
        assert page_count(doc) == expected
        data = doc.save_bytes()

    assert data.startswith(b"%PDF-")
    with vanillapdf.Document.open_bytes(data) as reopened:
        assert page_count(reopened) == expected


def test_open_stream_and_save_stream_round_trip(plain_pdf):
    with vanillapdf.Document.open(plain_pdf) as doc:
        expected = page_count(doc)

    with open(plain_pdf, "rb") as source:
        doc = vanillapdf.Document.open_stream(source)

    # The source is closed: objects still load lazily from the copied data.
    with doc:
        assert page_count(doc) == expected
        target = io.BytesIO()
        doc.save_stream(target)

    with vanillapdf.Document.open_bytes(target.getvalue()) as reopened:
        assert page_count(reopened) == expected


@pytest.mark.parametrize("save", ["save_incremental_bytes", "save_incremental_stream"])
def test_incremental_save_appends_to_original_data(plain_pdf, save):
    original = Path(plain_pdf).read_bytes()
    with vanillapdf.Document.open_bytes(original) as doc:
        expected = page_count(doc) + 1
        doc.get_catalog().get_pages().append(vanillapdf.PageObject.create_from_document(doc))
        if save == "save_incremental_bytes":
            data = doc.save_incremental_bytes()
        else:
            target = io.BytesIO()
            doc.save_incremental_stream(target)
            data = target.getvalue()

    assert data.startswith(original)
    with vanillapdf.Document.open_bytes(data) as reopened:
        assert page_count(reopened) == expected


def test_open_encrypted_bytes(test_pdf):
    with vanillapdf.Document.open_bytes(Path(test_pdf).read_bytes()) as doc:
        assert doc.get_catalog() is not None


def test_open_bytes_rejects_non_pdf():
    with pytest.raises(vanillapdf.PdfError):
        vanillapdf.Document.open_bytes(b"not a pdf")


def test_open_bytes_requires_bytes(test_pdf):
    with pytest.raises(TypeError):
        vanillapdf.Document.open_bytes(test_pdf)  # type: ignore[arg-type]


def test_open_stream_requires_binary_mode(test_pdf):
    with open(test_pdf, encoding="latin-1") as text, pytest.raises(TypeError):
        vanillapdf.Document.open_stream(text)  # type: ignore[arg-type]


@pytest.mark.parametrize("strategy", [
    vanillapdf.IOStrategyType.FILE_STREAM,
    vanillapdf.IOStrategyType.MEMORY,
])
def test_open_and_create_with_strategy(plain_pdf, tmp_path, strategy):
    with vanillapdf.Document.open(plain_pdf) as doc:
        expected = page_count(doc)

    target = tmp_path / "saved.pdf"
    with vanillapdf.Document.open(plain_pdf, strategy) as doc:
        assert page_count(doc) == expected
        doc.save(target)

    with vanillapdf.Document.open(target) as reopened:
        assert page_count(reopened) == expected

    created = tmp_path / "created.pdf"
    with vanillapdf.Document.create(created, strategy) as doc:
        doc.save(created)
    with vanillapdf.Document.open(created) as reopened:
        assert reopened.get_catalog() is not None


@pytest.mark.parametrize("strategy", [
    vanillapdf.IOStrategyType.UNDEFINED,
    vanillapdf.IOStrategyType.MEMORY_MAPPED,
])
def test_open_with_unusable_strategy_raises(plain_pdf, strategy):
    with pytest.raises(vanillapdf.PdfError):
        vanillapdf.Document.open(plain_pdf, strategy)
