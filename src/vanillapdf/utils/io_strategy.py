from enum import IntEnum


class IOStrategyType(IntEnum):
    """How a file on disk is read and written (mirrors native IOStrategyType)."""
    UNDEFINED = 0
    """No strategy; rejected by the library."""
    MEMORY = 1
    """Load the whole file into memory."""
    MEMORY_MAPPED = 2
    """Memory-mapped file access (not yet supported by the library)."""
    FILE_STREAM = 3
    """Streamed file access (the default)."""
