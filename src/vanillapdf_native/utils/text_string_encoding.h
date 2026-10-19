#ifndef VANILLAPDF_PY_UTILS_TEXT_STRING_ENCODING_H
#define VANILLAPDF_PY_UTILS_TEXT_STRING_ENCODING_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* TextStringEncoding_Detect(data: bytes) -> int (TextStringEncodingType) */
PyObject* text_string_encoding_detect(PyObject* self, PyObject* args);

/* TextStringEncoding_PDFDocEncodingByteToUnicode(byte: int) -> int (code point) */
PyObject* text_string_encoding_pdf_doc_encoding_byte_to_unicode(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_UTILS_TEXT_STRING_ENCODING_H */
