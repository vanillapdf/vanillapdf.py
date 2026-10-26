from vanillapdf import (
    Buffer,
    Document,
    DocumentSignatureSettings,
    File,
    InputOutputStream,
    MessageDigestAlgorithm,
    PKCS12Key,
)


def read_all(stream):
    stream.input_position = 0
    chunks = []
    while True:
        with stream.read_buffer(4096) as buffer:
            chunk = buffer.data
        if not chunk:
            return b"".join(chunks)
        chunks.append(chunk)


def test_write_and_read_buffer():
    with InputOutputStream.create_from_memory() as stream:
        with Buffer.create_from_data(b"%PDF-1.7") as buffer:
            stream.write_buffer(buffer)
        stream.flush()
        assert stream.output_position == 8
        with stream.read_buffer(8) as read:
            assert read.data == b"%PDF-1.7"


def test_file_streams_round_trip(plain_pdf):
    with Document.open(plain_pdf) as doc:
        expected = len(doc.get_catalog().get_pages())
        with InputOutputStream.create_from_memory() as stream:
            with File.create_stream(stream, "saved") as target:
                doc.save_file(target)
            with File.open_stream(stream, "saved") as source:
                source.initialize()
                assert source.get_filename() == "saved"
                with Document.open_file(source) as reopened:
                    assert len(reopened.get_catalog().get_pages()) == expected



def test_input_position():
    with InputOutputStream.create_from_memory() as stream:
        with Buffer.create_from_data(b"0123456789") as buffer:
            stream.write_buffer(buffer)
        stream.flush()
        stream.input_position = 4
        assert stream.input_position == 4
        with stream.read_buffer(3) as read:
            assert read.data == b"456"
        assert stream.input_position == 7
        with stream.read_buffer(100) as read:
            assert read.data == b"789"  # short read at the end


def test_sign_into_memory(plain_pdf, signing_key_path):
    """Signing reads back and patches its own output: neither stream position
    is left at the start or the end, so reading back must rewind."""
    with (
        PKCS12Key.create_from_file(signing_key_path, "test") as key,
        key.to_signing_key() as signing_key,
        DocumentSignatureSettings.create() as settings,
        Document.open(plain_pdf) as doc,
        InputOutputStream.create_from_memory() as stream,
    ):
        expected = len(doc.get_catalog().get_pages())
        settings.digest = MessageDigestAlgorithm.SHA256
        settings.set_signing_key(signing_key)
        with File.create_stream(stream, "signed") as destination:
            doc.sign(destination, settings)
        signed = read_all(stream)

    assert signed.startswith(b"%PDF-")
    with Document.open_bytes(signed) as reopened:
        assert len(reopened.get_catalog().get_pages()) == expected
