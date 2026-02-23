#ifndef VANILLAPDF_TEXT_STRING_ENCODING_MODULE_H
#define VANILLAPDF_TEXT_STRING_ENCODING_MODULE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

/* Detect the encoding of raw PDF text string bytes.
   Python signature: text_string_detect_encoding(data: bytes) -> int
   Returns a TextStringEncodingType integer (1=PDFDocEncoding, 2=UTF16BE, 3=UTF8).
   Used by the Python StringObject wrapper to implement get_text(). */
PyObject* text_string_detect_encoding(PyObject* self, PyObject* args);

/* Map a single PDFDocEncoding byte to its Unicode code point.
   Python signature: text_string_pdfdoc_to_unicode(byte: int) -> int
   Returns the Unicode code point. Undefined bytes return 0xFFFD (U+FFFD).
   Used by the Python StringObject wrapper to decode PDFDocEncoding strings. */
PyObject* text_string_pdfdoc_to_unicode(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_TEXT_STRING_ENCODING_MODULE_H */
