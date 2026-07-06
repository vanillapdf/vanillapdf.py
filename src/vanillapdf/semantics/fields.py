from __future__ import annotations

from collections.abc import Iterator
from enum import IntEnum
from typing import TYPE_CHECKING

from .. import _vanillapdf
from ..handle import Handle
from .digital_signature import DigitalSignature

if TYPE_CHECKING:
    from .._vanillapdf import FieldCollectionHandle, FieldHandle, SignatureFieldHandle


class FieldType(IntEnum):
    """Interactive form field type (mirrors native FieldType)."""
    UNDEFINED = 0
    NON_TERMINAL = 1
    BUTTON = 2
    TEXT = 3
    CHOICE = 4
    SIGNATURE = 5


class Field(Handle["FieldHandle"]):
    """A single interactive form field."""

    _release = staticmethod(_vanillapdf.field_release)

    def __init__(self, handle: FieldHandle) -> None:
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle: FieldHandle) -> Field:
        return cls(handle)

    @property
    def field_type(self) -> FieldType:
        handle = self._require_handle()
        return FieldType(_vanillapdf.field_get_type(handle))

    def as_signature_field(self) -> SignatureField | None:
        """Return this field as a :class:`SignatureField`, or ``None`` if it is
        not a signature field."""
        if self.field_type != FieldType.SIGNATURE:
            return None
        return SignatureField.from_field(self)


class SignatureField(Handle["SignatureFieldHandle"]):
    """A signature form field, holding a :class:`~vanillapdf.DigitalSignature`."""

    _release = staticmethod(_vanillapdf.signature_field_release)

    def __init__(self, handle: SignatureFieldHandle) -> None:
        self._handle = handle

    @staticmethod
    def from_field(field: Field) -> SignatureField:
        handle = _vanillapdf.signature_field_from_field(SignatureField._handle_of(field))
        return SignatureField(handle)

    @property
    def value(self) -> DigitalSignature:
        """The :class:`~vanillapdf.DigitalSignature` stored in this field."""
        handle = self._require_handle()
        signature = _vanillapdf.signature_field_get_value(handle)
        return DigitalSignature._from_handle(signature)


class FieldCollection(Handle["FieldCollectionHandle"]):
    """The interactive form's fields, as a 0-based Python sequence."""

    _release = staticmethod(_vanillapdf.field_collection_release)

    def __init__(self, handle: FieldCollectionHandle) -> None:
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle: FieldCollectionHandle) -> FieldCollection:
        return cls(handle)

    def __len__(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.field_collection_get_size(handle)

    def __getitem__(self, index: int) -> Field:
        size = len(self)
        if index < 0:
            index += size
        if index < 0 or index >= size:
            raise IndexError("field index out of range")
        handle = self._require_handle()
        field = _vanillapdf.field_collection_at(handle, index)
        return Field._from_handle(field)

    def __iter__(self) -> Iterator[Field]:
        for i in range(len(self)):
            yield self[i]
