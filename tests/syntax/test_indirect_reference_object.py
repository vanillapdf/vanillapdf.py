import pytest

from vanillapdf import IndirectReferenceObject, ObjectType


def test_create_and_release():
    obj = IndirectReferenceObject.create()
    assert obj._handle is not None
    obj.close()
    assert obj._handle is None
    obj.close()  # idempotent


def test_object_and_generation_number():
    with IndirectReferenceObject.create() as ref:
        assert ref.object_type == ObjectType.INDIRECT_REFERENCE
        ref.object_number = 12
        ref.generation_number = 3
        assert ref.object_number == 12
        assert ref.generation_number == 3


def test_use_after_close_raises():
    obj = IndirectReferenceObject.create()
    obj.close()
    with pytest.raises(ValueError):
        _ = obj.object_number
