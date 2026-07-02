from .. import _vanillapdf
from ..handle import Handle
from ..syntax.objects import Object, StringObject


class DocumentInfo(Handle):
    """The document information dictionary (title, author, ...).

    Each property returns a :class:`~vanillapdf.StringObject`, or ``None`` when
    that entry is absent from the document.
    """

    _release = staticmethod(_vanillapdf.document_info_release)

    def __init__(self, handle):
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle) -> "DocumentInfo":
        return cls(handle)

    def _string(self, native_getter):
        handle = self._require_handle()
        result = native_getter(handle)
        if result is None:
            return None
        return Object._wrap(result)

    @property
    def title(self):
        return self._string(_vanillapdf.document_info_get_title)

    @property
    def author(self):
        return self._string(_vanillapdf.document_info_get_author)

    @property
    def subject(self):
        return self._string(_vanillapdf.document_info_get_subject)

    @property
    def keywords(self):
        return self._string(_vanillapdf.document_info_get_keywords)

    @property
    def creator(self):
        return self._string(_vanillapdf.document_info_get_creator)

    @property
    def producer(self):
        return self._string(_vanillapdf.document_info_get_producer)
