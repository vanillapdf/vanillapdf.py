from pathlib import Path
import vanillapdf

def test_open_save_release():
    base = Path(__file__).parent

    test_input = base.parent / "assets" / "pdf-test.pdf"
    test_output = base / "test_output.pdf"

    assert test_input.exists(), f"{test_input} not found"

    with vanillapdf.Document(str(test_input)) as doc:
        assert doc is not None
        doc.save(str(test_output))
        assert test_output.exists()

    test_output.unlink()

test_open_save_release()
