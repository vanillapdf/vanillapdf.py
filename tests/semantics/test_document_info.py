import vanillapdf
from vanillapdf import StringObject


def test_document_info(test_pdf):
    with vanillapdf.Document(test_pdf) as doc:
        info = doc.get_document_info()
        assert info is not None

        producer = info.producer
        assert isinstance(producer, StringObject)
        assert b"Acrobat" in producer.value

        assert info.title is not None
