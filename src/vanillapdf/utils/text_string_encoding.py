from enum import IntEnum

from .. import _vanillapdf


class TextStringEncodingType(IntEnum):
    """Encoding of a PDF text string (mirrors native TextStringEncodingType).

    PDF text strings (PDF spec 7.9.2.2) are PDFDocEncoding when they carry no
    byte order mark, UTF-16BE with BOM ``FE FF``, or UTF-8 with BOM ``EF BB BF``
    (PDF 2.0).
    """
    UNDEFINED = 0
    PDF_DOC_ENCODING = 1
    UTF16BE = 2
    UTF8 = 3


class TextStringEncoding:
    """Utilities for working with PDF text string encodings (PDF spec 7.9.2.2)."""

    @staticmethod
    def detect(data: bytes) -> TextStringEncodingType:
        """Detect the encoding of raw text string bytes from their byte order mark.

        Data without a byte order mark is reported as
        :attr:`TextStringEncodingType.PDF_DOC_ENCODING`.
        """
        return TextStringEncodingType(_vanillapdf.text_string_encoding_detect(data))

    @staticmethod
    def pdf_doc_encoding_byte_to_unicode(value: int) -> int:
        """Map a single PDFDocEncoding byte (PDF spec Table D.2) to its Unicode code point.

        Bytes PDFDocEncoding leaves undefined map to U+FFFD. Raises
        :class:`OverflowError` if `value` is outside ``0..255``.
        """
        return _vanillapdf.text_string_encoding_pdf_doc_encoding_byte_to_unicode(value)

    @staticmethod
    def decode(data: bytes) -> str:
        """Decode the raw bytes of a PDF text string to ``str``.

        Uses the encoding the string declares (PDF spec 7.9.2.2): UTF-16BE or
        UTF-8 when it starts with that byte order mark (which is dropped, so it
        does not surface as a leading U+FEFF), otherwise PDFDocEncoding.
        Malformed or undefined bytes become U+FFFD rather than raising. This is
        the same decoding as .NET's ``PdfTextStringUtils.Decode``.
        """
        encoding = TextStringEncoding.detect(data)
        if encoding == TextStringEncodingType.UTF16BE:
            return data[2:].decode("utf-16-be", errors="replace")
        if encoding == TextStringEncodingType.UTF8:
            return data[3:].decode("utf-8", errors="replace")
        return "".join(
            chr(TextStringEncoding.pdf_doc_encoding_byte_to_unicode(byte)) for byte in data)
