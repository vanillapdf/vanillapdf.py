from .. import _vanillapdf
from ..handle import Handle
from .buffer import Buffer


class LicenseInfo:
    """Optional VanillaPDF license management.

    Setting a license is **opt-in and not required** — the library's
    functionality is not gated on it. These methods simply expose the library's
    license state for callers that want to inspect or set it.
    """

    @staticmethod
    def set_license_file(filename: str) -> None:
        """Load the license from a file on disk."""
        _vanillapdf.license_info_set_license_file(filename)

    @staticmethod
    def set_license_data(data: bytes) -> None:
        """Load the license from its file contents (``bytes``)."""
        with Buffer.create_from_data(data) as buffer:
            _vanillapdf.license_info_set_license_buffer(Handle._handle_of(buffer))

    @staticmethod
    def is_valid() -> bool:
        """Return True if a valid license is currently set."""
        return _vanillapdf.license_info_is_valid()

    @staticmethod
    def is_temporary() -> bool:
        """Return True if the current license is temporary (e.g. a trial)."""
        return _vanillapdf.license_info_is_temporary()
