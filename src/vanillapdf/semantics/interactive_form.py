from __future__ import annotations

from typing import TYPE_CHECKING

from .. import _vanillapdf
from ..handle import Handle
from .fields import FieldCollection

if TYPE_CHECKING:
    from .._vanillapdf import InteractiveFormHandle


class InteractiveForm(Handle["InteractiveFormHandle"]):
    """A document's interactive form (AcroForm)."""

    _release = staticmethod(_vanillapdf.interactive_form_release)

    def __init__(self, handle: InteractiveFormHandle) -> None:
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle: InteractiveFormHandle) -> InteractiveForm:
        return cls(handle)

    def get_fields(self) -> FieldCollection:
        handle = self._require_handle()
        fields = _vanillapdf.interactive_form_get_fields(handle)
        return FieldCollection._from_handle(fields)

    @property
    def fields(self) -> FieldCollection:
        return self.get_fields()
