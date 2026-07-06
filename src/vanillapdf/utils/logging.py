from enum import IntEnum

from .. import _vanillapdf


class LoggingSeverity(IntEnum):
    """Logging severity levels."""
    UNDEFINED = 0
    TRACE = 1
    DEBUG = 2
    INFO = 3
    WARNING = 4
    ERROR = 5
    CRITICAL = 6
    OFF = 7


class Logging:
    """Logging configuration for VanillaPDF.

    The native library defaults to writing its log to stdout, which corrupts
    saved files under output redirection, so at import the extension replaces
    that sink with a discarding one -- **native logs are silent by default**.

    To capture native logs (including verbose debug output), route them to a
    rotating file with :meth:`set_rotating_file`; it streams to disk natively, so
    it scales to heavy output and is safe under multi-threaded use.
    :meth:`set_severity` controls how verbose the native side is.
    """

    @staticmethod
    def get_severity() -> LoggingSeverity:
        """Get the current logging severity level."""
        return LoggingSeverity(_vanillapdf.logging_get_severity())

    @staticmethod
    def set_severity(severity: LoggingSeverity) -> None:
        """Set the logging severity level."""
        _vanillapdf.logging_set_severity(int(severity))

    @staticmethod
    def set_pattern(pattern: str) -> None:
        """Set the logging pattern format."""
        _vanillapdf.logging_set_pattern(pattern)

    @staticmethod
    def set_rotating_file(path: str, max_file_size: int = 5 * 1024 * 1024,
                          max_files: int = 3) -> None:
        """Route native logs to a rotating file sink at `path`.

        Replaces the default (discarding) sink, so native library output -- as
        verbose as :meth:`set_severity` allows -- is written to `path`, rolling
        over to a new file every `max_file_size` bytes and keeping `max_files`
        rotated files. Streamed to disk natively, so it scales to large debug
        output and is safe to use with threads.
        """
        _vanillapdf.logging_set_rotating_file(path, max_file_size, max_files)

    @staticmethod
    def shutdown() -> None:
        """Gracefully shutdown the logging system."""
        _vanillapdf.logging_shutdown()
