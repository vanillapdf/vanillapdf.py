#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_text_string_encoding.h>

#include "text_string_encodingmodule.h"

PyObject* text_string_detect_encoding(PyObject* self, PyObject* args) {
    const char* data;
    Py_ssize_t size;
    if (!PyArg_ParseTuple(args, "y#", &data, &size)) {
        return NULL;
    }

    TextStringEncodingType enc;
    error_type err = TextStringEncoding_Detect(
        (string_type)data, (size_type)size, &enc);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to detect text string encoding");
        return NULL;
    }

    return PyLong_FromLong((long)enc);
}

PyObject* text_string_pdfdoc_to_unicode(PyObject* self, PyObject* args) {
    unsigned int byte_val;
    if (!PyArg_ParseTuple(args, "I", &byte_val)) {
        return NULL;
    }

    if (byte_val > 255) {
        PyErr_SetString(PyExc_ValueError, "Byte value must be in range 0-255");
        return NULL;
    }

    uint32_t codepoint = 0;
    error_type err = TextStringEncoding_PDFDocToUnicode((uint8_t)byte_val, &codepoint);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to map PDFDocEncoding byte");
        return NULL;
    }

    return PyLong_FromUnsignedLong((unsigned long)codepoint);
}
