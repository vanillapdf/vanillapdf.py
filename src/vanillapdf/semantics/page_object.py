from __future__ import annotations

from typing import TYPE_CHECKING

from .. import _vanillapdf
from ..handle import Handle
from ..syntax.objects import Object
from .rectangle import Rectangle

if TYPE_CHECKING:
    from .._vanillapdf import PageObjectHandle
    from .document import Document


class PageObject(Handle["PageObjectHandle"]):
    """A single PDF page."""

    _release = staticmethod(_vanillapdf.page_object_release)

    def __init__(self, handle: PageObjectHandle) -> None:
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle: PageObjectHandle) -> PageObject:
        return cls(handle)

    @staticmethod
    def create_from_document(document: Document) -> PageObject:
        """Create a new (empty) page belonging to `document`."""
        page = _vanillapdf.page_object_create_from_document(PageObject._handle_of(document))
        return PageObject(page)

    @property
    def media_box(self) -> Rectangle:
        handle = self._require_handle()
        media_box = _vanillapdf.page_object_get_media_box(handle)
        return Rectangle._from_handle(media_box)

    @media_box.setter
    def media_box(self, rectangle: Rectangle) -> None:
        handle = self._require_handle()
        _vanillapdf.page_object_set_media_box(handle, self._handle_of(rectangle))

    @property
    def base_object(self) -> Object:
        """The underlying page dictionary."""
        handle = self._require_handle()
        base = _vanillapdf.page_object_get_base_object(handle)
        return Object._wrap(base)
