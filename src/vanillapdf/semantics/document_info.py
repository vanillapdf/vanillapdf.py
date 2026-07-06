from __future__ import annotations

from collections.abc import Callable
from typing import TYPE_CHECKING

from .. import _vanillapdf
from ..handle import Handle
from ..syntax.objects import StringObject

if TYPE_CHECKING:
    from .._vanillapdf import DocumentInfoHandle, ObjectHandle


class DocumentInfo(Handle["DocumentInfoHandle"]):
    """The document information dictionary (title, author, ...).

    Each property returns a :class:`~vanillapdf.StringObject`, or ``None`` when
    that entry is absent from the document.
    """

    _release = staticmethod(_vanillapdf.document_info_release)

    def __init__(self, handle: DocumentInfoHandle) -> None:
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle: DocumentInfoHandle) -> DocumentInfo:
        return cls(handle)

    def _string(
        self,
        native_getter: Callable[[DocumentInfoHandle], ObjectHandle | None],
    ) -> StringObject | None:
        handle = self._require_handle()
        result = native_getter(handle)
        if result is None:
            return None
        return StringObject(result)  # document info entries are string objects

    @property
    def title(self) -> StringObject | None:
        return self._string(_vanillapdf.document_info_get_title)

    @property
    def author(self) -> StringObject | None:
        return self._string(_vanillapdf.document_info_get_author)

    @property
    def subject(self) -> StringObject | None:
        return self._string(_vanillapdf.document_info_get_subject)

    @property
    def keywords(self) -> StringObject | None:
        return self._string(_vanillapdf.document_info_get_keywords)

    @property
    def creator(self) -> StringObject | None:
        return self._string(_vanillapdf.document_info_get_creator)

    @property
    def producer(self) -> StringObject | None:
        return self._string(_vanillapdf.document_info_get_producer)
