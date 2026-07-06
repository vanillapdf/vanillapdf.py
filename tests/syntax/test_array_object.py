import pytest

from vanillapdf import ArrayObject, IntegerObject, Object, ObjectType


def test_create_and_release():
    obj = ArrayObject.create()
    assert obj._handle is not None
    obj.close()
    assert obj._handle is None
    obj.close()  # idempotent


def test_array_object():
    with ArrayObject.create() as arr:
        assert arr.object_type == ObjectType.ARRAY
        assert len(arr) == 0
        for v in (1, 2, 3):
            arr.append(IntegerObject.create(v))
        assert len(arr) == 3
        assert isinstance(arr[0], IntegerObject)
        assert [o.value for o in arr] == [1, 2, 3]
        assert arr[-1].value == 3
        arr[0] = IntegerObject.create(9)
        assert arr[0].value == 9
        del arr[0]
        assert len(arr) == 2
        with pytest.raises(IndexError):
            _ = arr[10]


def test_array_insert_and_clear():
    with ArrayObject.create() as arr:
        arr.append(IntegerObject.create(1))
        arr.append(IntegerObject.create(3))
        arr.insert(1, IntegerObject.create(2))
        assert [o.value for o in arr] == [1, 2, 3]
        arr.clear()
        assert len(arr) == 0


def test_wrap_returns_array():
    arr = ArrayObject.create()
    wrapped = Object._wrap(arr._handle)
    arr._handle = None
    try:
        assert isinstance(wrapped, ArrayObject)
    finally:
        wrapped.close()


def test_use_after_close_raises():
    obj = ArrayObject.create()
    obj.close()
    with pytest.raises(ValueError):
        len(obj)
