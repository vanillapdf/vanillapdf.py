from __future__ import annotations

from collections.abc import Iterator
from typing import TYPE_CHECKING

from .. import _vanillapdf
from ..handle import Handle
from .page_object import PageObject

if TYPE_CHECKING:
    from .._vanillapdf import PageTreeHandle


class PageTree(Handle["PageTreeHandle"]):
    """The document's page tree.

    Indexing is 0-based and Pythonic (``pages[0]`` is the first page); the
    native page tree is 1-based, so indices are translated internally.
    """

    _release = staticmethod(_vanillapdf.page_tree_release)

    def __init__(self, handle: PageTreeHandle) -> None:
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle: PageTreeHandle) -> PageTree:
        return cls(handle)

    def __len__(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.page_tree_get_page_count(handle)

    def _native_index(self, index: int) -> int:
        """Translate a 0-based (possibly negative) index to the native 1-based
        page number, bounds-checked against the current page count."""
        size = len(self)
        if index < 0:
            index += size
        if index < 0 or index >= size:
            raise IndexError("page index out of range")
        return index + 1

    def __getitem__(self, index: int) -> PageObject:
        native_index = self._native_index(index)
        handle = self._require_handle()
        page = _vanillapdf.page_tree_get_page(handle, native_index)
        return PageObject._from_handle(page)

    def __delitem__(self, index: int) -> None:
        native_index = self._native_index(index)
        handle = self._require_handle()
        _vanillapdf.page_tree_remove_page(handle, native_index)

    def __iter__(self) -> Iterator[PageObject]:
        for i in range(len(self)):
            yield self[i]

    def append(self, page: PageObject) -> None:
        handle = self._require_handle()
        _vanillapdf.page_tree_append_page(handle, self._handle_of(page))

    def insert(self, index: int, page: PageObject) -> None:
        """Insert `page` so it becomes the element at 0-based `index`."""
        handle = self._require_handle()
        _vanillapdf.page_tree_insert_page(handle, index + 1, self._handle_of(page))
