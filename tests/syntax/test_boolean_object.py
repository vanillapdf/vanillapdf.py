import pytest

from vanillapdf import BooleanObject, ObjectType


def test_create_and_release():
    obj = BooleanObject.create(True)
    assert obj._handle is not None
    obj.close()
    assert obj._handle is None
    obj.close()  # idempotent


def test_value_get_set_and_serialize():
    with BooleanObject.create(True) as obj:
        assert obj.object_type == ObjectType.BOOLEAN
        assert obj.value is True
        obj.value = False
        assert obj.value is False
        assert obj.to_string() == b"false"


def test_use_after_close_raises():
    obj = BooleanObject.create(True)
    obj.close()
    with pytest.raises(ValueError):
        _ = obj.value
