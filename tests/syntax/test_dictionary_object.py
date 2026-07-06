import pytest

from vanillapdf import BooleanObject, DictionaryObject, IntegerObject, ObjectType


def test_create_and_release():
    obj = DictionaryObject.create()
    assert obj._handle is not None
    obj.close()
    assert obj._handle is None
    obj.close()  # idempotent


def test_dictionary_object():
    with DictionaryObject.create() as d:
        assert d.object_type == ObjectType.DICTIONARY
        assert len(d) == 0
        d["Type"] = IntegerObject.create(1)
        d["Count"] = IntegerObject.create(5)
        assert len(d) == 2
        assert "Type" in d
        assert "Missing" not in d
        assert d["Count"].value == 5
        assert d.get("Missing") is None
        assert d.get("Type").value == 1
        keys = d.keys()
        assert sorted(k.value_string() for k in keys) == ["Count", "Type"]
        items = {k.value_string(): v.value for k, v in d.items()}
        assert items == {"Type": 1, "Count": 5}
        del d["Type"]
        assert "Type" not in d
        with pytest.raises(KeyError):
            _ = d["Nope"]


def test_str_key():
    with DictionaryObject.create() as d:
        d["Root"] = BooleanObject.create(True)
        assert "Root" in d
        assert d["Root"].value is True


def test_use_after_close_raises():
    obj = DictionaryObject.create()
    obj.close()
    with pytest.raises(ValueError):
        len(obj)
