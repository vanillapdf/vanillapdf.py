from .. import _vanillapdf
from ..handle import Handle
from .page_tree import PageTree
from .interactive_form import InteractiveForm


class Catalog(Handle):
    """The document catalog (root of the PDF object hierarchy)."""

    _release = staticmethod(_vanillapdf.catalog_release)

    def __init__(self, handle):
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle) -> "Catalog":
        return cls(handle)

    def get_pages(self) -> PageTree:
        handle = self._require_handle()
        pages = _vanillapdf.catalog_get_pages(handle)
        return PageTree._from_handle(pages)

    @property
    def pages(self) -> PageTree:
        return self.get_pages()

    def get_acro_form(self):
        """The document's interactive form (:class:`~vanillapdf.InteractiveForm`),
        or ``None`` if the document has no AcroForm."""
        handle = self._require_handle()
        result = _vanillapdf.catalog_get_acro_form(handle)
        if result is None:
            return None
        return InteractiveForm._from_handle(result)
