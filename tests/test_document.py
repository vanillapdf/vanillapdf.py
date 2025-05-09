import vanillapdf
import os

def test_open_save_release():
    test_input = "existing.pdf"   # ensure this file exists for testing
    test_output = "test_output.pdf"

    doc = vanillapdf.Document(test_input)
    assert doc is not None

    doc.save(test_output)
    assert os.path.exists(test_output)

    doc.close()
    os.remove(test_output)

test_open_save_release()
print("Test passed.")