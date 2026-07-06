import vanillapdf


def test_buffer_roundtrip():
    data = b"hello world"
    with vanillapdf.Buffer.create_from_data(data) as buf:
        out = buf.get_data()
        assert out == data


def test_empty_create_and_set_data():
    with vanillapdf.Buffer.create() as buf:
        buf.set_data(b"foo")
        out = buf.get_data()
        assert out == b"foo"


test_buffer_roundtrip()
test_empty_create_and_set_data()

