import gc
import pytest
import vanillapdf
from vanillapdf import IntegerObject, Buffer, ObjectType, _vanillapdf


def test_double_close_is_idempotent():
    obj = IntegerObject.create(1)
    obj.close()
    obj.close()  # must not raise or double-release


def test_context_manager_closes():
    with IntegerObject.create(5) as obj:
        assert obj.value == 5
    with pytest.raises(ValueError):
        _ = obj.value


def test_exception_in_with_still_closes():
    obj = IntegerObject.create(1)
    with pytest.raises(RuntimeError):
        with obj:
            raise RuntimeError("boom")
    # __exit__ ran despite the exception, so the handle is released.
    with pytest.raises(ValueError):
        _ = obj.value


def test_garbage_collection_without_close():
    """Dropping the last reference must release the handle via __del__ without
    crashing or leaking."""
    obj = IntegerObject.create(1)
    assert obj.value == 1
    del obj
    gc.collect()  # __del__ -> object_release; must be clean


def test_wrong_capsule_type_raises_type_error():
    """Passing a Buffer capsule where an Object is expected is rejected by the
    capsule name check, not crashed."""
    buf = Buffer.create_from_data(b"x")
    try:
        with pytest.raises(TypeError):
            _vanillapdf.object_get_object_type(buf._handle)
    finally:
        buf.close()


def test_child_object_outlives_parent(test_pdf):
    """An indirect object holds its own reference, so it stays usable after the
    File it came from is closed."""
    f = vanillapdf.File(test_pdf)
    f.initialize()
    obj = f.get_indirect_object(2, 0)
    f.close()
    try:
        assert obj.object_type == ObjectType.DICTIONARY
    finally:
        obj.close()
