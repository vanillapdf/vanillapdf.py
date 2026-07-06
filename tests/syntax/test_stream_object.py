import pytest

from vanillapdf import DictionaryObject, IntegerObject, ObjectType, StreamObject


def test_create_and_release():
    obj = StreamObject.create()
    assert obj._handle is not None
    obj.close()
    assert obj._handle is None
    obj.close()  # idempotent


def test_body_roundtrip_and_header():
    with StreamObject.create() as stream:
        assert stream.object_type == ObjectType.STREAM
        # set_body stores the decoded body; body_raw is the encoded on-disk form
        # (populated only on serialization), so round-trip through body.
        stream.body = b"hello stream"
        assert stream.body == b"hello stream"
        assert isinstance(stream.header, DictionaryObject)


def test_header_setter():
    """The header setter routes a DictionaryObject handle through _handle_of."""
    with StreamObject.create() as stream, DictionaryObject.create() as header:
        header["Length"] = IntegerObject.create(0)
        stream.header = header
        assert "Length" in stream.header


def test_use_after_close_raises():
    obj = StreamObject.create()
    obj.close()
    with pytest.raises(ValueError):
        _ = obj.body
