from __future__ import annotations

from typing import TYPE_CHECKING

from .. import _vanillapdf
from ..handle import Handle
from .buffer import Buffer

if TYPE_CHECKING:
    from .._vanillapdf import InputOutputStreamHandle


class InputOutputStream(Handle["InputOutputStreamHandle"]):
    """A readable and writable native stream (mirrors native InputOutputStream).

    Pass one to :meth:`~vanillapdf.File.open_stream` or
    :meth:`~vanillapdf.File.create_stream` to use it as a file.
    """

    _release = staticmethod(_vanillapdf.input_output_stream_release)

    def __init__(self, handle: InputOutputStreamHandle) -> None:
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle: InputOutputStreamHandle) -> InputOutputStream:
        return cls(handle)

    @classmethod
    def create_from_memory(cls) -> InputOutputStream:
        """Create an empty stream backed by memory."""
        return cls(_vanillapdf.input_output_stream_create_from_memory())

    def read_buffer(self, length: int) -> Buffer:
        """Read up to `length` bytes from the input position."""
        handle = self._require_handle()
        return Buffer._from_handle(_vanillapdf.input_output_stream_read_buffer(handle, length))

    @property
    def output_position(self) -> int:
        """The position the next write goes to."""
        handle = self._require_handle()
        return _vanillapdf.input_output_stream_get_output_position(handle)

    def write_buffer(self, buffer: Buffer) -> None:
        """Write `buffer` at the output position."""
        handle = self._require_handle()
        _vanillapdf.input_output_stream_write_buffer(handle, self._handle_of(buffer))

    def flush(self) -> None:
        handle = self._require_handle()
        _vanillapdf.input_output_stream_flush(handle)
