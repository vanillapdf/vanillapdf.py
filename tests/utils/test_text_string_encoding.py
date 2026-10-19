import pytest

from vanillapdf import TextStringEncoding, TextStringEncodingType


def test_detect_utf8():
    data = b"\xef\xbb\xbfHello"
    assert TextStringEncoding.detect(data) == TextStringEncodingType.UTF8


def test_detect_utf16be():
    data = b"\xfe\xff\x00H\x00i"
    assert TextStringEncoding.detect(data) == TextStringEncodingType.UTF16BE


def test_detect_without_bom_is_pdf_doc_encoding():
    assert TextStringEncoding.detect(b"Hello world") == TextStringEncodingType.PDF_DOC_ENCODING


def test_pdf_doc_encoding_ascii_byte_maps_to_itself():
    assert TextStringEncoding.pdf_doc_encoding_byte_to_unicode(0x41) == 0x41
    assert TextStringEncoding.pdf_doc_encoding_byte_to_unicode(0x7A) == 0x7A


def test_pdf_doc_encoding_euro_sign():
    # PDF spec Table D.2: PDFDocEncoding 0xA0 is the euro sign
    assert TextStringEncoding.pdf_doc_encoding_byte_to_unicode(0xA0) == 0x20AC


@pytest.mark.parametrize("value", [-1, 256])
def test_pdf_doc_encoding_rejects_non_byte(value):
    with pytest.raises(OverflowError):
        TextStringEncoding.pdf_doc_encoding_byte_to_unicode(value)


def test_decode_utf16be_drops_bom():
    assert TextStringEncoding.decode(b"\xfe\xff\x00H\x00i") == "Hi"


def test_decode_utf8_drops_bom():
    assert TextStringEncoding.decode("\ufeffHéllo".encode("utf-8")) == "Héllo"


def test_decode_without_bom_uses_pdf_doc_encoding():
    # 0xA0 is the euro sign in PDFDocEncoding (Latin-1 would give U+00A0)
    assert TextStringEncoding.decode(b"Price \xa0") == "Price \u20ac"


def test_decode_empty():
    assert TextStringEncoding.decode(b"") == ""


def test_decode_malformed_utf16_replaces_instead_of_raising():
    # Odd byte count after the BOM: the dangling byte becomes U+FFFD.
    assert TextStringEncoding.decode(b"\xfe\xff\x00H\x00") == "H\ufffd"
