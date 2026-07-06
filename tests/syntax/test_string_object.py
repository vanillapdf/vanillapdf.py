import pytest

from vanillapdf import (
    Buffer,
    HexadecimalStringObject,
    LiteralStringObject,
    ObjectType,
    StringType,
)


def test_literal_create_and_release():
    obj = LiteralStringObject.create("Hello")
    assert obj._handle is not None
    obj.close()
    assert obj._handle is None
    obj.close()  # idempotent


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


def test_value_setter_via_buffer():
    """The value setter routes a Buffer handle through Handle._handle_of."""
    with LiteralStringObject.create("orig") as text, Buffer.create_from_data(b"updated") as buf:
        text.value = buf
        assert text.value == b"updated"


def test_use_after_close_raises():
    obj = LiteralStringObject.create("x")
    obj.close()
    with pytest.raises(ValueError):
        _ = obj.value
