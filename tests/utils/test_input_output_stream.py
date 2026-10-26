from vanillapdf import Buffer, Document, File, InputOutputStream


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

