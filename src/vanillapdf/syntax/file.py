from __future__ import annotations

import os
from enum import IntEnum
from typing import TYPE_CHECKING

from .. import _vanillapdf
from ..handle import Handle
from ..utils.buffer import Buffer
from ..utils.io_strategy import IOStrategyType
from .objects import Object

if TYPE_CHECKING:
    from .._vanillapdf import FileHandle
    from ..utils.input_output_stream import InputOutputStream


class PDFVersion(IntEnum):
    """PDF version identifiers."""
    UNDEFINED = 0
    PDF_1_0 = 1
    PDF_1_1 = 2
    PDF_1_2 = 3
    PDF_1_3 = 4
    PDF_1_4 = 5
    PDF_1_5 = 6
    PDF_1_6 = 7
    PDF_1_7 = 8
    PDF_2_0 = 9


class File(Handle["FileHandle"]):
    """Low-level PDF file access.

    Provides access to PDF structure including version and encryption status.
    Open one with :meth:`open` (then call :meth:`initialize` to read the xref
    tables) or create one with :meth:`create`.
    """

    _release = staticmethod(_vanillapdf.file_release)

    def __init__(self, handle: FileHandle) -> None:
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle: FileHandle) -> File:
        return cls(handle)

    @classmethod
    def open(cls, path: str | os.PathLike[str],
             strategy: IOStrategyType = IOStrategyType.FILE_STREAM) -> File:
        """Open the file at `path` for reading. Call :meth:`initialize` next."""
        handle = _vanillapdf.file_open(os.fspath(path), int(strategy))
        return cls._from_handle(handle)

    @classmethod
    def create(cls, path: str | os.PathLike[str],
               strategy: IOStrategyType = IOStrategyType.FILE_STREAM) -> File:
        """Create a new file for writing (e.g. a signing destination)."""
        handle = _vanillapdf.file_create(os.fspath(path), int(strategy))
        return cls._from_handle(handle)

    @classmethod
    def open_stream(cls, stream: InputOutputStream, name: str) -> File:
        """Open a file for reading from `stream`. Call :meth:`initialize` next."""
        handle = _vanillapdf.file_open_stream(cls._handle_of(stream), name)
        return cls._from_handle(handle)

    @classmethod
    def create_stream(cls, stream: InputOutputStream, name: str) -> File:
        """Create a file for writing into `stream`."""
        handle = _vanillapdf.file_create_stream(cls._handle_of(stream), name)
        return cls._from_handle(handle)

    def initialize(self) -> None:
        """Initialize the file by reading xref tables."""
        handle = self._require_handle()
        _vanillapdf.file_initialize(handle)

    def get_version(self) -> PDFVersion:
        """Get the PDF version as an enum."""
        handle = self._require_handle()
        version = _vanillapdf.file_get_version(handle)
        return PDFVersion(version)

    def get_version_string(self) -> str:
        """Get the PDF version as a string (e.g., '1.7', '2.0')."""
        version = self.get_version()
        version_map = {
            PDFVersion.PDF_1_0: "1.0",
            PDFVersion.PDF_1_1: "1.1",
            PDFVersion.PDF_1_2: "1.2",
            PDFVersion.PDF_1_3: "1.3",
            PDFVersion.PDF_1_4: "1.4",
            PDFVersion.PDF_1_5: "1.5",
            PDFVersion.PDF_1_6: "1.6",
            PDFVersion.PDF_1_7: "1.7",
            PDFVersion.PDF_2_0: "2.0",
        }
        return version_map.get(version, "unknown")

    def get_filename(self) -> str:
        """Get the filename the file was opened or created with."""
        handle = self._require_handle()
        buffer_handle = _vanillapdf.file_get_filename(handle)
        with Buffer._from_handle(buffer_handle) as buffer:
            filename_bytes = buffer.data
        return filename_bytes.decode("utf-8")

    @property
    def is_encrypted(self) -> bool:
        """Check if the file is encrypted."""
        handle = self._require_handle()
        return _vanillapdf.file_is_encrypted(handle)

    def set_encryption_password(self, password: str) -> None:
        """Set the password for an encrypted file."""
        handle = self._require_handle()
        _vanillapdf.file_set_encryption_password(handle, password)

    def get_indirect_object(self, object_number: int, generation_number: int = 0) -> Object:
        """Get an indirect object by its object and generation number."""
        handle = self._require_handle()
        result = _vanillapdf.file_get_indirect_object(handle, object_number, generation_number)
        return Object._wrap(result)
