import vanillapdf
from vanillapdf import StringObject


def test_document_info(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        info = doc.get_document_info()
        assert info is not None

        producer = info.producer
        assert isinstance(producer, StringObject)
        assert b"Acrobat" in producer.value

        assert info.title is not None


def test_text_decodes_pdf_doc_encoded_title(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        info = doc.get_document_info()
        assert info is not None and info.title is not None
        assert info.title.text == " PDF Test Page"


def test_text_decodes_utf16be_title(assets_dir):
    """A UTF-16BE title (BOM FE FF) decodes to text, with no leading U+FEFF."""
    path = str(assets_dir / "chinese_names_中文的名字-2.pdf")
    with vanillapdf.Document(path) as doc:
        info = doc.get_document_info()
        assert info is not None and info.title is not None
        assert info.title.value.startswith(b"\xfe\xff")
        assert info.title.text == "Chinese Names 中文的名字-2"
