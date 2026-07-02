from . import _vanillapdf
from .buffer import Buffer


class LicenseInfo:
    """Manage the VanillaPDF license.

    A valid license unlocks the full library; without one, licensed operations
    fail with :class:`~vanillapdf.errors.PdfError`.
    """

    @staticmethod
    def set_license_file(filename: str) -> None:
        """Load the license from a file on disk."""
        _vanillapdf.license_info_set_license_file(filename)

    @staticmethod
    def set_license_buffer(buffer: Buffer) -> None:
        """Load the license from an in-memory :class:`~vanillapdf.buffer.Buffer`."""
        _vanillapdf.license_info_set_license_buffer(buffer._handle)

    @staticmethod
    def is_valid() -> bool:
        """Return True if a valid license is currently set."""
        return _vanillapdf.license_info_is_valid()

    @staticmethod
    def is_temporary() -> bool:
        """Return True if the current license is temporary (e.g. a trial)."""
        return _vanillapdf.license_info_is_temporary()
