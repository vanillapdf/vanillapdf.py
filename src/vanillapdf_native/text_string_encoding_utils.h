#ifndef VANILLAPDF_TEXT_STRING_ENCODING_UTILS_H
#define VANILLAPDF_TEXT_STRING_ENCODING_UTILS_H

/*
 * Internal helpers for converting PDF text strings to/from Python str.
 *
 * These are static functions intended for inclusion in C modules that
 * deal with PDF text string fields (DocumentInfo, Annotation, Field, etc.).
 * They are never exposed directly to Python; callers return plain str/bytes.
 *
 * Requires: vanillapdf/utils/c_text_string_encoding.h
 *           vanillapdf/c_values.h
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_text_string_encoding.h>

/*
 * Convert raw PDF text string bytes to a Python str.
 *
 * Detects encoding by BOM inspection (PDFDocEncoding, UTF-16BE, UTF-8)
 * and converts transparently. For PDFDocEncoding, the authoritative mapping
 * table in the C library is used via per-byte lookup — no C-side allocation.
 * For UTF-16BE and UTF-8, Python's built-in codecs do the work.
 *
 * Returns a new str reference, or NULL with an exception set on failure.
 */
static PyObject* pdf_raw_text_to_pyunicode(const char* data, Py_ssize_t size) {
    TextStringEncodingType enc;
    error_type err = TextStringEncoding_Detect(
        (string_type)data, (size_type)size, &enc);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to detect text string encoding");
        return NULL;
    }

    switch (enc) {
    case TextStringEncodingType_UTF16BE: {
        /* BOM 0xFE 0xFF is present; pass the full buffer including BOM.
           byteorder=0 tells PyUnicode_DecodeUTF16 to read and consume it. */
        int byteorder = 0;
        return PyUnicode_DecodeUTF16(data, size, NULL, &byteorder);
    }
    case TextStringEncodingType_UTF8: {
        /* BOM 0xEF 0xBB 0xBF occupies 3 bytes; strip it and decode the rest. */
        if (size < 3) {
            return PyUnicode_FromStringAndSize("", 0);
        }
        return PyUnicode_DecodeUTF8(data + 3, size - 3, NULL);
    }
    default: { /* TextStringEncodingType_PDFDocEncoding */
        /* Per-byte lookup into the authoritative PDFDocEncoding table.
           No allocation in C; one call per byte into the mapping function.
           Undefined bytes (e.g. 0x01) produce U+FFFD (replacement character). */
        Py_UCS4* ucs4 = (Py_UCS4*)PyMem_Malloc((size_t)size * sizeof(Py_UCS4));
        if (!ucs4) {
            return PyErr_NoMemory();
        }
        for (Py_ssize_t i = 0; i < size; i++) {
            uint32_t cp = 0;
            TextStringEncoding_PDFDocToUnicode((uint8_t)data[i], &cp);
            ucs4[i] = (Py_UCS4)cp;
        }
        PyObject* result = PyUnicode_FromKindAndData(
            PyUnicode_4BYTE_KIND, ucs4, size);
        PyMem_Free(ucs4);
        return result;
    }
    }
}

/*
 * Encode a Python str for storage as a PDF text string.
 *
 * Always produces UTF-16BE with BOM (0xFE 0xFF prefix), which is valid for
 * all PDF versions and covers the full Unicode range. The caller receives a
 * bytes object ready to pass to StringObject_SetValue via a Buffer.
 *
 * Returns a new bytes reference, or NULL with an exception set on failure.
 */
static PyObject* pyunicode_to_pdf_text_bytes(PyObject* py_str) {
    if (!PyUnicode_Check(py_str)) {
        PyErr_SetString(PyExc_TypeError, "Expected str");
        return NULL;
    }

    /* Python's utf-16-be codec produces the content bytes without a BOM. */
    PyObject* encoded = PyUnicode_AsEncodedString(py_str, "utf-16-be", NULL);
    if (!encoded) return NULL;

    Py_ssize_t enc_size = PyBytes_GET_SIZE(encoded);
    const char* enc_data = PyBytes_AS_STRING(encoded);

    /* Prepend BOM: result = 0xFE 0xFF + utf-16-be content */
    PyObject* result = PyBytes_FromStringAndSize(NULL, 2 + enc_size);
    if (!result) {
        Py_DECREF(encoded);
        return NULL;
    }

    char* buf = PyBytes_AS_STRING(result);
    buf[0] = (char)0xFE;
    buf[1] = (char)0xFF;
    memcpy(buf + 2, enc_data, (size_t)enc_size);

    Py_DECREF(encoded);
    return result;
}

#endif /* VANILLAPDF_TEXT_STRING_ENCODING_UTILS_H */
