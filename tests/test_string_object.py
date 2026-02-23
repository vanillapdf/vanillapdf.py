"""
End-to-end tests for StringObject and TextStringEncoding.

Validates the design: StringObject exposes raw bytes, TextStringEncoding
converts between those bytes and Python str. The three PDF text string
encodings (PDFDocEncoding, UTF-16BE, UTF-8) are all exercised.
"""

import pytest
from vanillapdf.string_object import StringObject
from vanillapdf.text_string_encoding import TextStringEncoding


class TestTextStringEncodingDecode:
    def test_ascii_pdfdoc(self):
        # Pure ASCII has no BOM, falls through to PDFDocEncoding.
        # ASCII bytes map to themselves in both PDFDocEncoding and Unicode.
        assert TextStringEncoding.decode(b"Hello") == "Hello"

    def test_pdfdoc_high_byte(self):
        # 0x80 in PDFDocEncoding maps to U+2022 BULLET per PDF spec Table D.2.
        assert TextStringEncoding.decode(b"\x80") == "\u2022"

    def test_pdfdoc_mixed(self):
        assert TextStringEncoding.decode(b"Hi\x80") == "Hi\u2022"

    def test_utf16be_ascii(self):
        raw = b"\xfe\xff" + "Hello".encode("utf-16-be")
        assert TextStringEncoding.decode(raw) == "Hello"

    def test_utf16be_unicode(self):
        raw = b"\xfe\xff" + "中文".encode("utf-16-be")
        assert TextStringEncoding.decode(raw) == "中文"

    def test_utf8_ascii(self):
        raw = b"\xef\xbb\xbf" + "Hello".encode("utf-8")
        assert TextStringEncoding.decode(raw) == "Hello"

    def test_utf8_unicode(self):
        raw = b"\xef\xbb\xbf" + "中文".encode("utf-8")
        assert TextStringEncoding.decode(raw) == "中文"

    def test_empty(self):
        assert TextStringEncoding.decode(b"") == ""


class TestTextStringEncodingEncode:
    def test_ascii(self):
        result = TextStringEncoding.encode("Hello")
        assert result == b"\xfe\xff" + "Hello".encode("utf-16-be")

    def test_unicode(self):
        result = TextStringEncoding.encode("中文")
        assert result == b"\xfe\xff" + "中文".encode("utf-16-be")

    def test_empty(self):
        assert TextStringEncoding.encode("") == b"\xfe\xff"

    def test_roundtrip(self):
        for text in ("Hello", "中文", "Ünïcödé", "日本語テスト"):
            assert TextStringEncoding.decode(TextStringEncoding.encode(text)) == text


class TestStringObject:
    def test_create_and_read_raw(self):
        with StringObject.create(b"Hello") as so:
            assert so.data == b"Hello"

    def test_set_raw(self):
        with StringObject.create(b"Hello") as so:
            so.data = b"World"
            assert so.data == b"World"

    def test_get_text_pdfdoc(self):
        with StringObject.create(b"Hello") as so:
            assert TextStringEncoding.decode(so.data) == "Hello"

    def test_get_text_pdfdoc_high_byte(self):
        with StringObject.create(b"Hi\x80") as so:
            assert TextStringEncoding.decode(so.data) == "Hi\u2022"

    def test_get_text_utf16be(self):
        raw = b"\xfe\xff" + "Hello".encode("utf-16-be")
        with StringObject.create(raw) as so:
            assert TextStringEncoding.decode(so.data) == "Hello"

    def test_get_text_utf16be_unicode(self):
        raw = b"\xfe\xff" + "中文".encode("utf-16-be")
        with StringObject.create(raw) as so:
            assert TextStringEncoding.decode(so.data) == "中文"

    def test_set_text(self):
        with StringObject.create() as so:
            so.data = TextStringEncoding.encode("Hello")
            assert so.data == b"\xfe\xff" + "Hello".encode("utf-16-be")

    def test_roundtrip(self):
        for text in ("Hello", "中文", "Ünïcödé"):
            with StringObject.create() as so:
                so.data = TextStringEncoding.encode(text)
                assert TextStringEncoding.decode(so.data) == text

    def test_context_manager_releases(self):
        so = StringObject.create(b"Hello")
        so.close()
        # Second close should be a no-op, not an error
        so.close()
