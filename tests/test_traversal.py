from pathlib import Path
import vanillapdf
from vanillapdf import DictionaryObject, StreamObject, NameObject


def _test_pdf() -> str:
    return str(Path(__file__).parent.parent / "assets" / "pdf-test.pdf")


def test_get_indirect_object_dictionary():
    """Object 2 in the test asset is a dictionary; _wrap should return one."""
    with vanillapdf.File(_test_pdf()) as f:
        f.initialize()
        with f.get_indirect_object(2, 0) as obj:
            assert isinstance(obj, DictionaryObject)
            assert len(obj) > 0
            keys = obj.keys()
            assert all(isinstance(k, NameObject) for k in keys)


def test_get_indirect_object_stream():
    """Object 1 is a stream; header traversal and body decoding work through
    the binding (the asset is encrypted, so this also exercises decryption)."""
    with vanillapdf.File(_test_pdf()) as f:
        f.initialize()
        with f.get_indirect_object(1, 0) as stream:
            assert isinstance(stream, StreamObject)
            header = stream.header
            assert isinstance(header, DictionaryObject)
            assert "Length" in header
            body = stream.body
            assert isinstance(body, bytes)
            assert len(body) > 0
