from . import _vanillapdf

class Buffer:
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
        _vanillapdf.buffer_set_data(self._handle, data)

    def get_data(self) -> bytes:
        return _vanillapdf.buffer_get_data(self._handle)

    @property
    def data(self) -> bytes:
        """Get the buffer data."""
        return self.get_data()

    def close(self):
        if self._handle is not None:
            _vanillapdf.buffer_release(self._handle)
            self._handle = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def __del__(self):
        self.close()

