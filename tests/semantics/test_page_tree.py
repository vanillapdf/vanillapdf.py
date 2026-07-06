import pytest

import vanillapdf
from vanillapdf import PageObject


def test_iteration_and_bounds(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        pages = doc.get_catalog().get_pages()
        collected = list(pages)
        assert len(collected) == len(pages) == 1
        assert isinstance(pages[-1], PageObject)  # negative index
        with pytest.raises(IndexError):
            _ = pages[5]
