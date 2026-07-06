from __future__ import annotations

from typing import TYPE_CHECKING

from .. import _vanillapdf
from ..handle import Handle

if TYPE_CHECKING:
    from .._vanillapdf import RectangleHandle


class Rectangle(Handle["RectangleHandle"]):
    """A PDF rectangle (e.g. a page media box). Coordinates are integers."""

    _release = staticmethod(_vanillapdf.rectangle_release)

    def __init__(self, handle: RectangleHandle) -> None:
        self._handle = handle

    @staticmethod
    def create() -> Rectangle:
        handle = _vanillapdf.rectangle_create()
        return Rectangle(handle)

    @classmethod
    def _from_handle(cls, handle: RectangleHandle) -> Rectangle:
        return cls(handle)

    @property
    def lower_left_x(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.rectangle_get_lower_left_x(handle)

    @lower_left_x.setter
    def lower_left_x(self, value: int) -> None:
        handle = self._require_handle()
        _vanillapdf.rectangle_set_lower_left_x(handle, value)

    @property
    def lower_left_y(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.rectangle_get_lower_left_y(handle)

    @lower_left_y.setter
    def lower_left_y(self, value: int) -> None:
        handle = self._require_handle()
        _vanillapdf.rectangle_set_lower_left_y(handle, value)

    @property
    def upper_right_x(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.rectangle_get_upper_right_x(handle)

    @upper_right_x.setter
    def upper_right_x(self, value: int) -> None:
        handle = self._require_handle()
        _vanillapdf.rectangle_set_upper_right_x(handle, value)

    @property
    def upper_right_y(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.rectangle_get_upper_right_y(handle)

    @upper_right_y.setter
    def upper_right_y(self, value: int) -> None:
        handle = self._require_handle()
        _vanillapdf.rectangle_set_upper_right_y(handle, value)

    @property
    def width(self) -> int:
        return self.upper_right_x - self.lower_left_x

    @property
    def height(self) -> int:
        return self.upper_right_y - self.lower_left_y

    def __repr__(self) -> str:
        if self._handle is None:
            return "<Rectangle (closed)>"
        return (f"<Rectangle [{self.lower_left_x} {self.lower_left_y} "
                f"{self.upper_right_x} {self.upper_right_y}]>")
