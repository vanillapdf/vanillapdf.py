from . import _vanillapdf


class File:
    """Low-level PDF file access.

    Provides access to PDF structure including version and encryption status.
    Must call initialize() after opening to read xref tables.
    """

    def __init__(self, filename: str):
        self._handle = _vanillapdf.file_open(filename)
        if self._handle is None:
            raise RuntimeError(f"Failed to open file '{filename}'")

    def initialize(self) -> None:
        """Initialize the file by reading xref tables."""
        if self._handle is None:
            raise ValueError("File has been closed")
        _vanillapdf.file_initialize(self._handle)

    @property
    def version(self) -> str:
        """Get the PDF version (e.g., '1.7', '2.0')."""
        if self._handle is None:
            raise ValueError("File has been closed")
        return _vanillapdf.file_get_version(self._handle)

    @property
    def filename(self) -> str:
        """Get the filename."""
        if self._handle is None:
            raise ValueError("File has been closed")
        return _vanillapdf.file_get_filename(self._handle)

    @property
    def is_encrypted(self) -> bool:
        """Check if the file is encrypted."""
        if self._handle is None:
            raise ValueError("File has been closed")
        return _vanillapdf.file_is_encrypted(self._handle)

    def set_encryption_password(self, password: str) -> None:
        """Set the password for an encrypted file."""
        if self._handle is None:
            raise ValueError("File has been closed")
        _vanillapdf.file_set_encryption_password(self._handle, password)

    def close(self) -> None:
        """Release the file handle."""
        if self._handle is not None:
            _vanillapdf.file_release(self._handle)
            self._handle = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def __del__(self):
        self.close()
