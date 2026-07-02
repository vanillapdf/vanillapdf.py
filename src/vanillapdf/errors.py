from . import _vanillapdf

#: Exception raised when a VanillaPDF library call fails. Instances carry the
#: numeric library error code in the ``error_code`` attribute.
PdfError = _vanillapdf.PdfError


class Errors:
    """Access to the library's thread-local last-error state.

    Note: the library's ``VANILLAPDF_ERROR_*`` codes are link-time constants,
    not stable public numbers, so compare against :meth:`get_printable_error_text`
    rather than hard-coding numeric values.
    """

    @staticmethod
    def get_last_error() -> int:
        """Get the last error code recorded on the current thread."""
        return _vanillapdf.errors_get_last_error()

    @staticmethod
    def get_printable_error_text(code: int) -> str:
        """Get the human-readable name for an error code."""
        return _vanillapdf.errors_get_printable_error_text(code)

    @staticmethod
    def get_last_error_message() -> str:
        """Get the detail message for the last error on the current thread."""
        return _vanillapdf.errors_get_last_error_message()
