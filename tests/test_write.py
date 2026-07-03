from vanillapdf import Document, PageObject, Rectangle


def test_document_create_add_page_and_save(tmp_path):
    """Exercise the write path: create a document, build a page with a media
    box, append it, save, and reopen."""
    target = str(tmp_path / "created.pdf")

    doc = Document.create(target)
    try:
        page = PageObject.create_from_document(doc)

        rect = Rectangle.create()
        rect.lower_left_x = 0
        rect.lower_left_y = 0
        rect.upper_right_x = 200
        rect.upper_right_y = 300
        page.media_box = rect

        pages = doc.get_catalog().get_pages()
        before = len(pages)
        pages.append(page)
        assert len(pages) == before + 1

        doc.save(target)
    finally:
        doc.close()

    assert (tmp_path / "created.pdf").stat().st_size > 0

    # Reopen and confirm it is a valid document with pages.
    with Document(target) as reopened:
        assert len(reopened.get_catalog().get_pages()) >= 1
