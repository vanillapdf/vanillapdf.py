import vanillapdf
from vanillapdf import Catalog, InteractiveForm, PageTree


def test_get_pages(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        catalog = doc.get_catalog()
        assert isinstance(catalog, Catalog)
        pages = catalog.get_pages()
        assert isinstance(pages, PageTree)
        assert len(pages) == 1


def test_pages_property(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        catalog = doc.get_catalog()
        assert catalog.pages is not None


def test_get_acro_form(test_pdf):
    """The AcroForm accessor is callable and returns None or an InteractiveForm."""
    with vanillapdf.Document(test_pdf) as doc:
        result = doc.get_catalog().get_acro_form()
        assert result is None or isinstance(result, InteractiveForm)
