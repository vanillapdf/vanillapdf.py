from . import _vanillapdf
from .handle import Handle


class Document(Handle):
    _release = staticmethod(_vanillapdf.document_release)

    def __init__(self, filename: str = None):
        if filename:
            self._handle = _vanillapdf.document_open(filename)
        else:
            raise NotImplementedError("Currently only opening existing documents is supported.")

    def save(self, filename: str):
        handle = self._require_handle()
        _vanillapdf.document_save(handle, filename)
