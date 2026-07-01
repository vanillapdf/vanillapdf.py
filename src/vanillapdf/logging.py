from enum import IntEnum
from . import _vanillapdf


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
    """Logging configuration for VanillaPDF."""

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
    def shutdown() -> None:
        """Gracefully shutdown the logging system."""
        _vanillapdf.logging_shutdown()
