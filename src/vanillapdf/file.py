from enum import IntEnum
from . import _vanillapdf
from .buffer import Buffer
from .handle import Handle


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


class File(Handle):
    """Low-level PDF file access.

    Provides access to PDF structure including version and encryption status.
    Must call initialize() after opening to read xref tables.
    """

    _release = staticmethod(_vanillapdf.file_release)

    def __init__(self, filename: str):
        self._handle = _vanillapdf.file_open(filename)

    def initialize(self) -> None:
        """Initialize the file by reading xref tables."""
        _vanillapdf.file_initialize(self._require_handle())

    def get_version(self) -> PDFVersion:
        """Get the PDF version as an enum."""
        return PDFVersion(_vanillapdf.file_get_version(self._require_handle()))

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

    def get_filename(self) -> Buffer:
        """Get the filename as a Buffer."""
        buffer_handle = _vanillapdf.file_get_filename(self._require_handle())
        return Buffer._from_handle(buffer_handle)

    def get_filename_string(self, encoding: str = 'utf-8') -> str:
        """Get the filename as a string.

        Args:
            encoding: The encoding to use for decoding (default: utf-8).

        Returns:
            The filename as a decoded string.
        """
        buffer = self.get_filename()
        return buffer.data.decode(encoding)

    @property
    def is_encrypted(self) -> bool:
        """Check if the file is encrypted."""
        return _vanillapdf.file_is_encrypted(self._require_handle())

    def set_encryption_password(self, password: str) -> None:
        """Set the password for an encrypted file."""
        _vanillapdf.file_set_encryption_password(self._require_handle(), password)
