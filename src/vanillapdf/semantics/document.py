from __future__ import annotations

import os
from collections.abc import Callable
from typing import TYPE_CHECKING, BinaryIO

from .. import _vanillapdf
from ..handle import Handle
from ..syntax.file import File
from ..utils.buffer import Buffer
from ..utils.input_output_stream import InputOutputStream
from ..utils.io_strategy import IOStrategyType
from .catalog import Catalog
from .document_encryption_settings import DocumentEncryptionSettings
from .document_info import DocumentInfo

if TYPE_CHECKING:
    from .._vanillapdf import DocumentHandle
    from .document_signature_settings import DocumentSignatureSettings

# Name the library gives a file backed by memory instead of a path.
_MEMORY_FILE_NAME = "<memory>"


class Document(Handle["DocumentHandle"]):
    """A PDF document (the high-level, semantic view of a file).

    Construct one with :meth:`open`, :meth:`open_bytes`, :meth:`open_stream`,
    :meth:`open_file` or :meth:`create`.
    """

    _release = staticmethod(_vanillapdf.document_release)

    def __init__(self, handle: DocumentHandle) -> None:
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle: DocumentHandle) -> Document:
        return cls(handle)

    @classmethod
    def open(cls, path: str | os.PathLike[str],
             strategy: IOStrategyType = IOStrategyType.FILE_STREAM) -> Document:
        """Open the document at `path`."""
        handle = _vanillapdf.document_open(os.fspath(path), int(strategy))
        return cls._from_handle(handle)

    @classmethod
    def open_bytes(cls, data: bytes) -> Document:
        """Open a document from its bytes. The data is copied."""
        with InputOutputStream.create_from_memory() as stream:
            with Buffer.create_from_data(data) as buffer:
                stream.write_buffer(buffer)
            stream.flush()
            # The file keeps the stream alive, and the document keeps the file.
            with File.open_stream(stream, _MEMORY_FILE_NAME) as file:
                return cls.open_file(file)

    @classmethod
    def open_stream(cls, stream: BinaryIO) -> Document:
        """Open a document from a readable binary stream, read to its end.

        The data is copied; `stream` may be closed once this returns.
        """
        return cls.open_bytes(stream.read())

    @classmethod
    def open_file(cls, file: File) -> Document:
        """Open a document from an already-open :class:`~vanillapdf.File`."""
        handle = _vanillapdf.document_open_file(cls._handle_of(file))
        return cls._from_handle(handle)

    @classmethod
    def create(cls, path: str | os.PathLike[str],
               strategy: IOStrategyType = IOStrategyType.FILE_STREAM) -> Document:
        """Create a new, empty document backed by `path`."""
        handle = _vanillapdf.document_create(os.fspath(path), int(strategy))
        return cls._from_handle(handle)

    def save(self, path: str | os.PathLike[str]) -> None:
        """Save the document to `path`."""
        handle = self._require_handle()
        _vanillapdf.document_save(handle, os.fspath(path))

    def save_incremental(self, path: str | os.PathLike[str]) -> None:
        """Save the original data with the changes appended to `path`."""
        handle = self._require_handle()
        _vanillapdf.document_save_incremental(handle, os.fspath(path))

    def save_stream(self, stream: BinaryIO) -> None:
        """Write the saved document to a writable binary stream."""
        stream.write(self.to_bytes())

    def save_incremental_stream(self, stream: BinaryIO) -> None:
        """Write the incrementally saved document to a writable binary stream."""
        stream.write(self.to_bytes_incremental())

    def save_file(self, file: File) -> None:
        """Save the document into `file` (see :meth:`File.create_stream`)."""
        handle = self._require_handle()
        _vanillapdf.document_save_file(handle, self._handle_of(file))

    def save_incremental_file(self, file: File) -> None:
        """Save the original data with the changes appended into `file`."""
        handle = self._require_handle()
        _vanillapdf.document_save_incremental_file(handle, self._handle_of(file))

    def to_bytes(self) -> bytes:
        """Return the saved document as bytes."""
        return self._save_to_memory(self.save_file)

    def to_bytes_incremental(self) -> bytes:
        """Return the incrementally saved document as bytes."""
        return self._save_to_memory(self.save_incremental_file)

    @staticmethod
    def _save_to_memory(save: Callable[[File], None]) -> bytes:
        with InputOutputStream.create_from_memory() as stream:
            with File.create_stream(stream, _MEMORY_FILE_NAME) as file:
                save(file)
            # Saving writes sequentially, so the output position is the size.
            with stream.read_buffer(stream.output_position) as buffer:
                return buffer.data

    def get_catalog(self) -> Catalog:
        handle = self._require_handle()
        catalog = _vanillapdf.document_get_catalog(handle)
        return Catalog._from_handle(catalog)

    def get_document_info(self) -> DocumentInfo | None:
        """The document info dictionary, or None if the document has none."""
        handle = self._require_handle()
        result = _vanillapdf.document_get_document_info(handle)
        if result is None:
            return None
        return DocumentInfo._from_handle(result)

    def append_document(self, source: Document) -> None:
        handle = self._require_handle()
        _vanillapdf.document_append_document(handle, self._handle_of(source))

    def add_encryption(self, settings: DocumentEncryptionSettings) -> None:
        """Encrypt the document using `settings`. Takes effect on the next save."""
        handle = self._require_handle()
        _vanillapdf.document_add_encryption(handle, self._handle_of(settings))

    def remove_encryption(self) -> None:
        """Remove encryption from the document. Takes effect on the next save."""
        handle = self._require_handle()
        _vanillapdf.document_remove_encryption(handle)

    def sign(self, destination: File, settings: DocumentSignatureSettings) -> None:
        """Sign the document, writing the signed result to `destination`.

        `destination` is a writable :class:`~vanillapdf.File` (see
        :meth:`~vanillapdf.File.create`); `settings` is a
        :class:`~vanillapdf.DocumentSignatureSettings`. Unlike :meth:`save`,
        signing writes directly to the destination file.
        """
        handle = self._require_handle()
        _vanillapdf.document_sign(
            handle, self._handle_of(destination), self._handle_of(settings))
