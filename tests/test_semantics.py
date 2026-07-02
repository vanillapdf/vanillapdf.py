import pytest
import vanillapdf
from vanillapdf import (
    Catalog,
    PageTree,
    PageObject,
    Rectangle,
    DictionaryObject,
    StringObject,
)


def test_catalog_pages_media_box(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        catalog = doc.get_catalog()
        assert isinstance(catalog, Catalog)
        pages = catalog.get_pages()
        assert isinstance(pages, PageTree)
        assert len(pages) == 1

        page = pages[0]
        assert isinstance(page, PageObject)

        media_box = page.media_box
        assert isinstance(media_box, Rectangle)
        coords = (media_box.lower_left_x, media_box.lower_left_y,
                  media_box.upper_right_x, media_box.upper_right_y)
        assert coords == (0, 0, 612, 792)
        assert media_box.width == 612
        assert media_box.height == 792


def test_page_tree_iteration_and_bounds(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        pages = doc.get_catalog().get_pages()
        collected = list(pages)
        assert len(collected) == len(pages) == 1
        assert isinstance(pages[-1], PageObject)  # negative index
        with pytest.raises(IndexError):
            _ = pages[5]


def test_page_base_object_is_dictionary(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        page = doc.get_catalog().get_pages()[0]
        base = page.base_object
        assert isinstance(base, DictionaryObject)
        assert "Type" in base


def test_rectangle_coordinates():
    with Rectangle.create() as rect:
        rect.lower_left_x = 10
        rect.lower_left_y = 20
        rect.upper_right_x = 110
        rect.upper_right_y = 220
        assert rect.lower_left_x == 10
        assert rect.lower_left_y == 20
        assert rect.upper_right_x == 110
        assert rect.upper_right_y == 220
        assert rect.width == 100
        assert rect.height == 200
        assert "Rectangle" in repr(rect)


def test_document_info(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        info = doc.get_document_info()
        assert info is not None

        producer = info.producer
        assert isinstance(producer, StringObject)
        assert b"Acrobat" in producer.value

        assert info.title is not None
