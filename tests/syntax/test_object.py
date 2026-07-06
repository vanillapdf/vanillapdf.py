"""Base :class:`Object` behaviour: type dispatch, serialization, lifecycle."""
import pytest

from vanillapdf import (
    ArrayObject,
    BooleanObject,
    HexadecimalStringObject,
    IntegerObject,
    LiteralStringObject,
    NameObject,
    NullObject,
    Object,
    ObjectType,
    PdfError,
    RealObject,
    _vanillapdf,
)


def test_type_name():
    assert Object.type_name(ObjectType.NULL) == "Null"


def test_wrap_dispatches_to_subclass():
    # An object created as Integer, when re-wrapped from its handle, comes back
    # as an IntegerObject.
    obj = IntegerObject.create(5)
    wrapped = Object._wrap(obj._handle)
    obj._handle = None  # detach so we don't double-release the shared handle
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
    (lambda: ArrayObject.create(), ArrayObject),
])
def test_wrap_dispatch_all(factory, expected_cls):
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


def test_repr():
    obj = IntegerObject.create(7)
    assert "IntegerObject" in repr(obj)
    obj.close()
    assert "closed" in repr(obj)


def test_use_after_close_raises():
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
