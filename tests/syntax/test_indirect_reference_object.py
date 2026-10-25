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


@pytest.mark.parametrize("value", [-1, 2**64 + 5])
def test_object_number_rejects_out_of_range(value):
    # Must raise, not wrap: -1 used to become 2**64 - 1 and 2**64 + 5 became 5.
    with IndirectReferenceObject.create() as ref:
        ref.object_number = 12
        with pytest.raises(OverflowError):
            ref.object_number = value
        assert ref.object_number == 12
