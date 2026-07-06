import pytest

from vanillapdf import NullObject, ObjectType


def test_create_and_release():
    obj = NullObject.create()
    assert obj._handle is not None
    obj.close()
    assert obj._handle is None
    obj.close()  # idempotent


def test_null_object():
    with NullObject.create() as obj:
        assert obj.object_type == ObjectType.NULL
        assert obj.to_string() == b"null"


def test_use_after_close_raises():
    obj = NullObject.create()
    obj.close()
    with pytest.raises(ValueError):
        _ = obj.object_type
