import vanillapdf


def test_buffer_roundtrip():
    data = b"hello world"
    buf = vanillapdf.Buffer.create_from_data(data)
    out = buf.get_data()
    assert out == data
    buf.close()


def test_empty_create_and_set_data():
    buf = vanillapdf.Buffer.create()
    buf.set_data(b"foo")
    out = buf.get_data()
    assert out == b"foo"
    buf.close()

test_buffer_roundtrip()
test_empty_create_and_set_data()

