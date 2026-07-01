from datetime import date
from . import _vanillapdf


class LibraryInfo:
    """Information about the VanillaPDF library."""

    @staticmethod
    def get_version_major() -> int:
        """Get the major version number."""
        return _vanillapdf.library_info_get_version_major()

    @staticmethod
    def get_version_minor() -> int:
        """Get the minor version number."""
        return _vanillapdf.library_info_get_version_minor()

    @staticmethod
    def get_version_patch() -> int:
        """Get the patch version number."""
        return _vanillapdf.library_info_get_version_patch()

    @staticmethod
    def get_version_build() -> int:
        """Get the build version number."""
        return _vanillapdf.library_info_get_version_build()

    @staticmethod
    def get_author() -> str:
        """Get the library author."""
        return _vanillapdf.library_info_get_author()

    @staticmethod
    def get_build_day() -> int:
        """Get the build day."""
        return _vanillapdf.library_info_get_build_day()

    @staticmethod
    def get_build_month() -> int:
        """Get the build month."""
        return _vanillapdf.library_info_get_build_month()

    @staticmethod
    def get_build_year() -> int:
        """Get the build year."""
        return _vanillapdf.library_info_get_build_year()

    @staticmethod
    def get_version() -> str:
        """Get the full version string (e.g., '2.2.0.0')."""
        return f"{LibraryInfo.get_version_major()}.{LibraryInfo.get_version_minor()}.{LibraryInfo.get_version_patch()}.{LibraryInfo.get_version_build()}"

    @staticmethod
    def get_build_date() -> date:
        """Get the build date."""
        return date(
            LibraryInfo.get_build_year(),
            LibraryInfo.get_build_month(),
            LibraryInfo.get_build_day()
        )
