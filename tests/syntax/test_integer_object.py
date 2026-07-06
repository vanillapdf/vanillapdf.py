import pytest

from vanillapdf import IntegerObject, ObjectType


def test_create_and_release():
    obj = IntegerObject.create(42)
    assert obj._handle is not None
    obj.close()
    assert obj._handle is None
    obj.close()  # idempotent


def test_value_get_set_and_serialize():
    with IntegerObject.create(42) as obj:
        assert obj.object_type == ObjectType.INTEGER
        assert obj.value == 42
        obj.value = -7
        assert obj.value == -7
        assert obj.to_string() == b"-7"


def test_value_64bit():
    big = 2**62
    with IntegerObject.create(big) as obj:
        assert obj.value == big


def test_use_after_close_raises():
    obj = IntegerObject.create(1)
    obj.close()
    with pytest.raises(ValueError):
        _ = obj.value
