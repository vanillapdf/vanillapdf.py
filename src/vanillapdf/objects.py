from enum import IntEnum
from . import _vanillapdf
from .handle import Handle
from .buffer import Buffer


class ObjectType(IntEnum):
    """PDF syntax object types (mirrors native ObjectType)."""
    UNDEFINED = 0
    NULL = 1
    ARRAY = 2
    BOOLEAN = 3
    DICTIONARY = 4
    INTEGER = 5
    NAME = 6
    REAL = 7
    STREAM = 8
    STRING = 9
    INDIRECT_REFERENCE = 10


class StringType(IntEnum):
    """String object subtypes (mirrors native StringType)."""
    UNDEFINED = 0
    LITERAL = 1
    HEXADECIMAL = 2


class Object(Handle):
    """Base class for a PDF syntax object.

    Instances own a native "VanillaPDF.Object" handle. Use :meth:`_wrap` to
    turn a raw handle into the most specific subclass based on its type.
    """

    _release = staticmethod(_vanillapdf.object_release)

    def __init__(self, handle):
        self._handle = handle

    @property
    def object_type(self) -> ObjectType:
        """The object's :class:`ObjectType`."""
        handle = self._require_handle()
        return ObjectType(_vanillapdf.object_get_object_type(handle))

    @property
    def offset(self) -> int:
        """The object's byte offset within the file (0 if not applicable)."""
        handle = self._require_handle()
        return _vanillapdf.object_get_offset(handle)

    def to_string(self) -> bytes:
        """A human-readable representation of the object."""
        handle = self._require_handle()
        buffer_handle = _vanillapdf.object_to_string(handle)
        return Buffer._from_handle(buffer_handle).data

    def to_pdf(self) -> bytes:
        """The object serialized as it would appear in a PDF file."""
        handle = self._require_handle()
        buffer_handle = _vanillapdf.object_to_pdf(handle)
        return Buffer._from_handle(buffer_handle).data

    @staticmethod
    def type_name(object_type: ObjectType) -> str:
        """The printable name of an :class:`ObjectType`."""
        return _vanillapdf.object_type_name(object_type)

    @classmethod
    def _wrap(cls, handle) -> "Object":
        """Wrap a raw object handle in the most specific subclass."""
        object_type = ObjectType(_vanillapdf.object_get_object_type(handle))

        if object_type == ObjectType.STRING:
            string_type = StringType(_vanillapdf.string_object_get_string_type(handle))
            subclass = _STRING_TYPE_MAP.get(string_type, StringObject)
            return subclass(handle)

        subclass = _OBJECT_TYPE_MAP.get(object_type, Object)
        return subclass(handle)

    def __repr__(self):
        if self._handle is None:
            return f"<{type(self).__name__} (closed)>"
        return f"<{type(self).__name__} {self.object_type.name}>"


class NullObject(Object):
    @staticmethod
    def create() -> "NullObject":
        return NullObject(_vanillapdf.null_object_create())


class BooleanObject(Object):
    @staticmethod
    def create(value: bool) -> "BooleanObject":
        return BooleanObject(_vanillapdf.boolean_object_create(value))

    @property
    def value(self) -> bool:
        handle = self._require_handle()
        return _vanillapdf.boolean_object_get_value(handle)

    @value.setter
    def value(self, value: bool) -> None:
        handle = self._require_handle()
        _vanillapdf.boolean_object_set_value(handle, value)


class IntegerObject(Object):
    @staticmethod
    def create(value: int) -> "IntegerObject":
        return IntegerObject(_vanillapdf.integer_object_create(value))

    @property
    def value(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.integer_object_get_value(handle)

    @value.setter
    def value(self, value: int) -> None:
        handle = self._require_handle()
        _vanillapdf.integer_object_set_value(handle, value)


class RealObject(Object):
    @staticmethod
    def create(value: float, precision: int = 6) -> "RealObject":
        return RealObject(_vanillapdf.real_object_create(value, precision))

    @property
    def value(self) -> float:
        handle = self._require_handle()
        return _vanillapdf.real_object_get_value(handle)

    @value.setter
    def value(self, value: float) -> None:
        handle = self._require_handle()
        _vanillapdf.real_object_set_value(handle, value)


class NameObject(Object):
    @staticmethod
    def create(value: str) -> "NameObject":
        return NameObject(_vanillapdf.name_object_create_from_decoded_string(value))

    @property
    def value(self) -> bytes:
        handle = self._require_handle()
        buffer_handle = _vanillapdf.name_object_get_value(handle)
        return Buffer._from_handle(buffer_handle).data

    def value_string(self, encoding: str = "utf-8") -> str:
        return self.value.decode(encoding)


class StringObject(Object):
    @property
    def string_type(self) -> StringType:
        handle = self._require_handle()
        return StringType(_vanillapdf.string_object_get_string_type(handle))

    @property
    def value(self) -> bytes:
        handle = self._require_handle()
        buffer_handle = _vanillapdf.string_object_get_value(handle)
        return Buffer._from_handle(buffer_handle).data

    @value.setter
    def value(self, buffer: Buffer) -> None:
        handle = self._require_handle()
        _vanillapdf.string_object_set_value(handle, buffer._handle)

    def value_string(self, encoding: str = "utf-8") -> str:
        return self.value.decode(encoding)


class LiteralStringObject(StringObject):
    @staticmethod
    def create(value: str) -> "LiteralStringObject":
        return LiteralStringObject(
            _vanillapdf.literal_string_object_create_from_decoded_string(value))


class HexadecimalStringObject(StringObject):
    @staticmethod
    def create(value: str) -> "HexadecimalStringObject":
        return HexadecimalStringObject(
            _vanillapdf.hexadecimal_string_object_create_from_decoded_string(value))


_OBJECT_TYPE_MAP = {
    ObjectType.NULL: NullObject,
    ObjectType.BOOLEAN: BooleanObject,
    ObjectType.INTEGER: IntegerObject,
    ObjectType.REAL: RealObject,
    ObjectType.NAME: NameObject,
    ObjectType.STRING: StringObject,
    # ARRAY / DICTIONARY / STREAM / INDIRECT_REFERENCE arrive with the
    # container layer; until then _wrap falls back to the base Object.
}

_STRING_TYPE_MAP = {
    StringType.LITERAL: LiteralStringObject,
    StringType.HEXADECIMAL: HexadecimalStringObject,
}
