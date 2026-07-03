from .. import _vanillapdf
from ..handle import Handle
from .catalog import Catalog
from .document_encryption_settings import DocumentEncryptionSettings
from .document_info import DocumentInfo


class Document(Handle):
    """A PDF document (the high-level, semantic view of a file)."""

    _release = staticmethod(_vanillapdf.document_release)

    def __init__(self, filename: str = None):
        if filename:
            self._handle = _vanillapdf.document_open(filename)
        else:
            raise NotImplementedError(
                "Use Document.create() or Document.open_file() to construct without a filename.")

    @classmethod
    def _from_handle(cls, handle) -> "Document":
        document = cls.__new__(cls)
        document._handle = handle
        return document

    @staticmethod
    def create(filename: str) -> "Document":
        """Create a new, empty document backed by `filename`."""
        handle = _vanillapdf.document_create(filename)
        return Document._from_handle(handle)

    @staticmethod
    def open_file(file) -> "Document":
        """Open a document from an already-open :class:`~vanillapdf.File`."""
        handle = _vanillapdf.document_open_file(file._handle)
        return Document._from_handle(handle)

    def save(self, filename: str) -> None:
        handle = self._require_handle()
        _vanillapdf.document_save(handle, filename)

    def save_incremental(self, filename: str) -> None:
        handle = self._require_handle()
        _vanillapdf.document_save_incremental(handle, filename)

    def get_catalog(self) -> Catalog:
        handle = self._require_handle()
        catalog = _vanillapdf.document_get_catalog(handle)
        return Catalog._from_handle(catalog)

    def get_document_info(self):
        """The document info dictionary, or None if the document has none."""
        handle = self._require_handle()
        result = _vanillapdf.document_get_document_info(handle)
        if result is None:
            return None
        return DocumentInfo._from_handle(result)

    def append_document(self, source: "Document") -> None:
        handle = self._require_handle()
        _vanillapdf.document_append_document(handle, source._handle)

    def add_encryption(self, settings: DocumentEncryptionSettings) -> None:
        """Encrypt the document using `settings`. Takes effect on the next save."""
        handle = self._require_handle()
        _vanillapdf.document_add_encryption(handle, settings._handle)

    def remove_encryption(self) -> None:
        """Remove encryption from the document. Takes effect on the next save."""
        handle = self._require_handle()
        _vanillapdf.document_remove_encryption(handle)

    def sign(self, destination, settings) -> None:
        """Sign the document, writing the signed result to `destination`.

        `destination` is a writable :class:`~vanillapdf.File` (see
        :meth:`~vanillapdf.File.create`); `settings` is a
        :class:`~vanillapdf.DocumentSignatureSettings`. Unlike :meth:`save`,
        signing writes directly to the destination file.
        """
        handle = self._require_handle()
        _vanillapdf.document_sign(handle, destination._handle, settings._handle)
