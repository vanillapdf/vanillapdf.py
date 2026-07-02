from . import _vanillapdf


class MiscUtils:
    """Miscellaneous library-level utilities."""

    @staticmethod
    def initialize_openssl() -> None:
        """Initialize the OpenSSL backend used for cryptographic operations."""
        _vanillapdf.misc_utils_initialize_openssl()

    @staticmethod
    def cleanup_openssl() -> None:
        """Tear down the OpenSSL backend previously initialized."""
        _vanillapdf.misc_utils_cleanup_openssl()
