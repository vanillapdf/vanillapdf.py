from . import _vanillapdf
from .handle import Handle


class Buffer(Handle):
    _release = staticmethod(_vanillapdf.buffer_release)

    def __init__(self, handle) -> None:
        if handle is None:
            raise RuntimeError("Failed to create buffer")
        self._handle = handle

    @staticmethod
    def create() -> "Buffer":
        """Create an empty buffer."""
        handle = _vanillapdf.buffer_create()
        return Buffer(handle)

    @staticmethod
    def create_from_data(data: bytes) -> "Buffer":
        """Create a buffer from the provided bytes."""
        handle = _vanillapdf.buffer_create_from_data(data)
        return Buffer(handle)

    @classmethod
    def _from_handle(cls, handle) -> "Buffer":
        """Create a Buffer from an existing handle (internal use)."""
        return cls(handle)

    def set_data(self, data: bytes) -> None:
        handle = self._require_handle()
        _vanillapdf.buffer_set_data(handle, data)

    def get_data(self) -> bytes:
        handle = self._require_handle()
        return _vanillapdf.buffer_get_data(handle)

    @property
    def data(self) -> bytes:
        """Get the buffer data."""
        return self.get_data()
