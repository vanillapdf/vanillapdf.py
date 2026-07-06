import pytest

from vanillapdf import NameObject, ObjectType


def test_create_and_release():
    obj = NameObject.create("Type")
    assert obj._handle is not None
    obj.close()
    assert obj._handle is None
    obj.close()  # idempotent


def test_value():
    with NameObject.create("Type") as obj:
        assert obj.object_type == ObjectType.NAME
        assert obj.value == b"Type"
        assert obj.value_string() == "Type"


def test_use_after_close_raises():
    obj = NameObject.create("Type")
    obj.close()
    with pytest.raises(ValueError):
        _ = obj.value
