import pytest
import vanillapdf
from vanillapdf import (
    _vanillapdf,
    PdfError,
    ObjectType,
    StringType,
    Object,
    NullObject,
    BooleanObject,
    IntegerObject,
    RealObject,
    NameObject,
    StringObject,
    LiteralStringObject,
    HexadecimalStringObject,
)


def test_null_object():
    with NullObject.create() as obj:
        assert obj.object_type == ObjectType.NULL
        assert Object.type_name(ObjectType.NULL) == "Null"
        assert obj.to_string() == b"null"


def test_boolean_object():
    with BooleanObject.create(True) as obj:
        assert obj.object_type == ObjectType.BOOLEAN
        assert obj.value is True
        obj.value = False
        assert obj.value is False
        assert obj.to_string() == b"false"


def test_integer_object():
    with IntegerObject.create(42) as obj:
        assert obj.object_type == ObjectType.INTEGER
        assert obj.value == 42
        obj.value = -7
        assert obj.value == -7
        assert obj.to_string() == b"-7"


def test_integer_object_64bit():
    big = 2**62
    with IntegerObject.create(big) as obj:
        assert obj.value == big


def test_real_object():
    with RealObject.create(3.5, 2) as obj:
        assert obj.object_type == ObjectType.REAL
        assert obj.value == 3.5


def test_name_object():
    with NameObject.create("Type") as obj:
        assert obj.object_type == ObjectType.NAME
        assert obj.value == b"Type"
        assert obj.value_string() == "Type"


def test_literal_string_object():
    with LiteralStringObject.create("Hello") as obj:
        assert obj.object_type == ObjectType.STRING
        assert obj.string_type == StringType.LITERAL
        assert obj.value == b"Hello"


def test_hexadecimal_string_object():
    with HexadecimalStringObject.create("Hi") as obj:
        assert obj.object_type == ObjectType.STRING
        assert obj.string_type == StringType.HEXADECIMAL
        assert obj.value == b"Hi"


def test_wrap_dispatches_to_subclass():
    # An object created as Integer, when re-wrapped from its handle, should
    # come back as an IntegerObject.
    obj = IntegerObject.create(5)
    wrapped = Object._wrap(obj._handle)
    # Detach the original so we don't double-release the shared handle.
    obj._handle = None
    try:
        assert isinstance(wrapped, IntegerObject)
        assert wrapped.value == 5
    finally:
        wrapped.close()


def test_wrap_string_dispatches_to_hex():
    obj = HexadecimalStringObject.create("Hi")
    wrapped = Object._wrap(obj._handle)
    obj._handle = None
    try:
        assert isinstance(wrapped, HexadecimalStringObject)
    finally:
        wrapped.close()


@pytest.mark.parametrize("factory, expected_cls", [
    (lambda: NullObject.create(), NullObject),
    (lambda: BooleanObject.create(True), BooleanObject),
    (lambda: IntegerObject.create(1), IntegerObject),
    (lambda: RealObject.create(1.0, 2), RealObject),
    (lambda: NameObject.create("X"), NameObject),
    (lambda: LiteralStringObject.create("x"), LiteralStringObject),
    (lambda: HexadecimalStringObject.create("x"), HexadecimalStringObject),
])
def test_wrap_dispatch_all_scalars(factory, expected_cls):
    obj = factory()
    wrapped = Object._wrap(obj._handle)
    obj._handle = None  # transfer ownership of the shared handle to `wrapped`
    try:
        assert isinstance(wrapped, expected_cls)
    finally:
        wrapped.close()


def test_to_pdf():
    with IntegerObject.create(7) as obj:
        assert obj.to_pdf() == b"7"


def test_offset_is_int():
    # A freshly constructed object is not tied to a file offset.
    with IntegerObject.create(7) as obj:
        assert isinstance(obj.offset, int)


def test_object_use_after_close_raises():
    obj = IntegerObject.create(1)
    obj.close()
    with pytest.raises(ValueError):
        _ = obj.value


def test_wrong_type_accessor_raises_pdf_error():
    """Reading an Integer value from a Boolean object must fail in the library
    (IntegerObject_FromObject) and surface as PdfError, not garbage."""
    with BooleanObject.create(True) as boolean:
        with pytest.raises(PdfError) as exc:
            _vanillapdf.integer_object_get_value(boolean._handle)
        assert isinstance(exc.value.error_code, int)


def test_repr():
    obj = IntegerObject.create(7)
    assert "IntegerObject" in repr(obj)
    obj.close()
    assert "closed" in repr(obj)
