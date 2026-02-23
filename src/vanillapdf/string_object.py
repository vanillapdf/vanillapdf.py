from . import _vanillapdf


class StringObject:
    """A PDF string object.

    Exposes the raw decoded bytes of the string. Text interpretation
    (encoding detection, PDFDocEncoding conversion) is the responsibility
    of TextStringEncoding.
    """

    def __init__(self, handle) -> None:
        self._handle = handle

    @classmethod
    def create(cls, data: bytes = b"") -> "StringObject":
        """Create a LiteralStringObject from raw decoded bytes."""
        handle = _vanillapdf.literal_string_object_create(data)
        return cls(handle)

    @property
    def data(self) -> bytes:
        """Raw decoded bytes of the string object."""
        return _vanillapdf.string_object_get_raw(self._handle)

    @data.setter
    def data(self, value: bytes) -> None:
        _vanillapdf.string_object_set_raw(self._handle, value)

    def close(self) -> None:
        if self._handle is not None:
            _vanillapdf.string_object_release(self._handle)
            self._handle = None

    def __enter__(self) -> "StringObject":
        return self

    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        self.close()

    def __del__(self) -> None:
        self.close()
