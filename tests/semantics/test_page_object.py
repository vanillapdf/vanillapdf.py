import vanillapdf
from vanillapdf import DictionaryObject, Rectangle


def test_media_box(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        page = doc.get_catalog().get_pages()[0]
        media_box = page.media_box
        assert isinstance(media_box, Rectangle)
        coords = (media_box.lower_left_x, media_box.lower_left_y,
                  media_box.upper_right_x, media_box.upper_right_y)
        assert coords == (0, 0, 612, 792)
        assert media_box.width == 612
        assert media_box.height == 792


def test_base_object_is_dictionary(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        page = doc.get_catalog().get_pages()[0]
        base = page.base_object
        assert isinstance(base, DictionaryObject)
        assert "Type" in base
