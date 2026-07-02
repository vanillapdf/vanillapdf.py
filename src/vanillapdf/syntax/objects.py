from enum import IntEnum
from .. import _vanillapdf
from ..handle import Handle
from ..utils.buffer import Buffer


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


class ArrayObject(Object):
    @staticmethod
    def create() -> "ArrayObject":
        return ArrayObject(_vanillapdf.array_object_create())

    def __len__(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.array_object_get_size(handle)

    def _normalize_index(self, index: int) -> int:
        size = len(self)
        if index < 0:
            index += size
        if index < 0 or index >= size:
            raise IndexError("array index out of range")
        return index

    def __getitem__(self, index: int) -> Object:
        index = self._normalize_index(index)
        handle = self._require_handle()
        return Object._wrap(_vanillapdf.array_object_get_value(handle, index))

    def __setitem__(self, index: int, value: Object) -> None:
        index = self._normalize_index(index)
        handle = self._require_handle()
        _vanillapdf.array_object_set_value(handle, index, value._handle)

    def __delitem__(self, index: int) -> None:
        index = self._normalize_index(index)
        handle = self._require_handle()
        _vanillapdf.array_object_remove(handle, index)

    def __iter__(self):
        for i in range(len(self)):
            yield self[i]

    def append(self, value: Object) -> None:
        handle = self._require_handle()
        _vanillapdf.array_object_append(handle, value._handle)

    def insert(self, index: int, value: Object) -> None:
        handle = self._require_handle()
        _vanillapdf.array_object_insert(handle, index, value._handle)

    def clear(self) -> None:
        handle = self._require_handle()
        _vanillapdf.array_object_clear(handle)


class _DictionaryIterator(Handle):
    _release = staticmethod(_vanillapdf.dictionary_iterator_release)

    def __init__(self, handle):
        self._handle = handle

    def is_valid(self) -> bool:
        return _vanillapdf.dictionary_iterator_is_valid(self._require_handle())

    def key(self) -> Object:
        return Object._wrap(_vanillapdf.dictionary_iterator_get_key(self._require_handle()))

    def value(self) -> Object:
        return Object._wrap(_vanillapdf.dictionary_iterator_get_value(self._require_handle()))

    def advance(self) -> None:
        _vanillapdf.dictionary_iterator_next(self._require_handle())


class DictionaryObject(Object):
    @staticmethod
    def create() -> "DictionaryObject":
        return DictionaryObject(_vanillapdf.dictionary_object_create())

    def __len__(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.dictionary_object_get_size(handle)

    @staticmethod
    def _as_name(key):
        """Return (NameObject, owned) for a str or NameObject key. `owned` is
        True when we created a temporary that the caller must close."""
        if isinstance(key, NameObject):
            return key, False
        if isinstance(key, str):
            return NameObject.create(key), True
        raise TypeError(
            f"dictionary key must be str or NameObject, not {type(key).__name__}")

    def __contains__(self, key) -> bool:
        name, owned = self._as_name(key)
        try:
            handle = self._require_handle()
            return _vanillapdf.dictionary_object_contains(handle, name._handle)
        finally:
            if owned:
                name.close()

    def get(self, key, default=None):
        name, owned = self._as_name(key)
        try:
            handle = self._require_handle()
            result = _vanillapdf.dictionary_object_try_find(handle, name._handle)
        finally:
            if owned:
                name.close()
        if result is None:
            return default
        return Object._wrap(result)

    def __getitem__(self, key) -> Object:
        value = self.get(key)
        if value is None:
            raise KeyError(key)
        return value

    def __setitem__(self, key, value: Object) -> None:
        name, owned = self._as_name(key)
        try:
            handle = self._require_handle()
            _vanillapdf.dictionary_object_insert(handle, name._handle, value._handle, True)
        finally:
            if owned:
                name.close()

    def __delitem__(self, key) -> None:
        name, owned = self._as_name(key)
        try:
            handle = self._require_handle()
            removed = _vanillapdf.dictionary_object_remove(handle, name._handle)
        finally:
            if owned:
                name.close()
        if not removed:
            raise KeyError(key)

    def clear(self) -> None:
        handle = self._require_handle()
        _vanillapdf.dictionary_object_clear(handle)

    def items(self):
        handle = self._require_handle()
        iterator = _DictionaryIterator(_vanillapdf.dictionary_object_get_iterator(handle))
        try:
            while iterator.is_valid():
                yield iterator.key(), iterator.value()
                iterator.advance()
        finally:
            iterator.close()

    def keys(self):
        return [key for key, _ in self.items()]

    def values(self):
        return [value for _, value in self.items()]

    def __iter__(self):
        for key, _ in self.items():
            yield key


class StreamObject(Object):
    @staticmethod
    def create() -> "StreamObject":
        return StreamObject(_vanillapdf.stream_object_create())

    @property
    def header(self) -> "DictionaryObject":
        handle = self._require_handle()
        return Object._wrap(_vanillapdf.stream_object_get_header(handle))

    @header.setter
    def header(self, value: "DictionaryObject") -> None:
        handle = self._require_handle()
        _vanillapdf.stream_object_set_header(handle, value._handle)

    @property
    def body(self) -> bytes:
        """The decoded (filters applied) stream body."""
        handle = self._require_handle()
        return Buffer._from_handle(_vanillapdf.stream_object_get_body(handle)).data

    @body.setter
    def body(self, data: bytes) -> None:
        handle = self._require_handle()
        buffer = Buffer.create_from_data(data)
        try:
            _vanillapdf.stream_object_set_body(handle, buffer._handle)
        finally:
            buffer.close()

    @property
    def body_raw(self) -> bytes:
        """The raw (undecoded) stream body."""
        handle = self._require_handle()
        return Buffer._from_handle(_vanillapdf.stream_object_get_body_raw(handle)).data


class IndirectReferenceObject(Object):
    @staticmethod
    def create() -> "IndirectReferenceObject":
        return IndirectReferenceObject(_vanillapdf.indirect_reference_object_create())

    @property
    def object_number(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.indirect_reference_object_get_referenced_object_number(handle)

    @object_number.setter
    def object_number(self, value: int) -> None:
        handle = self._require_handle()
        _vanillapdf.indirect_reference_object_set_object_number(handle, value)

    @property
    def generation_number(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.indirect_reference_object_get_referenced_generation_number(handle)

    @generation_number.setter
    def generation_number(self, value: int) -> None:
        handle = self._require_handle()
        _vanillapdf.indirect_reference_object_set_generation_number(handle, value)

    @property
    def referenced_object(self) -> Object:
        handle = self._require_handle()
        return Object._wrap(_vanillapdf.indirect_reference_object_get_referenced_object(handle))

    @referenced_object.setter
    def referenced_object(self, value: Object) -> None:
        handle = self._require_handle()
        _vanillapdf.indirect_reference_object_set_referenced_object(handle, value._handle)


_OBJECT_TYPE_MAP = {
    ObjectType.NULL: NullObject,
    ObjectType.BOOLEAN: BooleanObject,
    ObjectType.INTEGER: IntegerObject,
    ObjectType.REAL: RealObject,
    ObjectType.NAME: NameObject,
    ObjectType.STRING: StringObject,
    ObjectType.ARRAY: ArrayObject,
    ObjectType.DICTIONARY: DictionaryObject,
    ObjectType.STREAM: StreamObject,
    ObjectType.INDIRECT_REFERENCE: IndirectReferenceObject,
}

_STRING_TYPE_MAP = {
    StringType.LITERAL: LiteralStringObject,
    StringType.HEXADECIMAL: HexadecimalStringObject,
}
