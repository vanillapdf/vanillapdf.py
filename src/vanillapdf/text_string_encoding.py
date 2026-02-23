from . import _vanillapdf

# Matches TextStringEncodingType in c_text_string_encoding.h
_ENCODING_PDFDOC = 1
_ENCODING_UTF16BE = 2
_ENCODING_UTF8 = 3


class TextStringEncoding:
    """Utilities for decoding and encoding PDF text strings.

    PDF text strings may be encoded as PDFDocEncoding (no BOM), UTF-16BE
    (BOM 0xFE 0xFF), or UTF-8 (BOM 0xEF 0xBB 0xBF, PDF 2.0).

    Typical use with StringObject:

        text = TextStringEncoding.decode(string_obj.data)
        string_obj.data = TextStringEncoding.encode("new value")
    """

    @staticmethod
    def decode(data: bytes) -> str:
        """Decode raw PDF text string bytes to a Python str.

        Detects encoding automatically from the BOM. For PDFDocEncoding,
        uses the authoritative mapping table from the C library.
        """
        enc = _vanillapdf.text_string_detect_encoding(data)

        if enc == _ENCODING_UTF16BE:
            # BOM 0xFE 0xFF is present; Python's utf-16 codec reads and strips it.
            return data.decode("utf-16")

        if enc == _ENCODING_UTF8:
            # BOM 0xEF 0xBB 0xBF occupies 3 bytes; strip and decode the rest.
            return data[3:].decode("utf-8")

        # PDFDocEncoding: per-byte lookup into the C library's mapping table.
        return "".join(
            chr(_vanillapdf.text_string_pdfdoc_to_unicode(b)) for b in data
        )

    @staticmethod
    def encode(text: str) -> bytes:
        """Encode a Python str as a PDF text string (UTF-16BE with BOM).

        Always uses UTF-16BE with BOM, which is valid for all PDF versions
        and covers the full Unicode range.
        """
        return b"\xfe\xff" + text.encode("utf-16-be")
