import pytest

from vanillapdf import ObjectType, RealObject


def test_create_and_release():
    obj = RealObject.create(3.5, 2)
    assert obj._handle is not None
    obj.close()
    assert obj._handle is None
    obj.close()  # idempotent


def test_value_get_set():
    with RealObject.create(3.5, 2) as obj:
        assert obj.object_type == ObjectType.REAL
        assert obj.value == 3.5
        obj.value = 1.25
        assert obj.value == 1.25


def test_use_after_close_raises():
    obj = RealObject.create(1.0)
    obj.close()
    with pytest.raises(ValueError):
        _ = obj.value
