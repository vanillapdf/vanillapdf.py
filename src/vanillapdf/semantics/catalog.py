from .. import _vanillapdf
from ..handle import Handle
from .page_tree import PageTree


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
