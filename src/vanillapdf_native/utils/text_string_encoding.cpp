#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_text_string_encoding.h>

#include "utils/text_string_encoding.h"
#include "common.h"

PyObject* text_string_encoding_detect(PyObject* self, PyObject* args) {
    const char* data;
    Py_ssize_t size;
    if (!PyArg_ParseTuple(args, "y#", &data, &size)) {
        return nullptr;
    }

    size_type data_size = 0;
    if (safe_convert(size, &data_size) < 0) {
        return nullptr;
    }

    TextStringEncodingType result = TextStringEncodingType_Undefined;
    error_type err = without_gil([data, data_size, &result] {
        return TextStringEncoding_Detect(data, data_size, &result);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "TextStringEncoding_Detect");
    }

    return to_python(result);
}

PyObject* text_string_encoding_pdf_doc_encoding_byte_to_unicode(PyObject* self, PyObject* args) {
    int value;
    if (!PyArg_ParseTuple(args, "i", &value)) {
        return nullptr;
    }

    uint8_t byte = 0;
    if (safe_convert(value, &byte) < 0) {
        return nullptr;
    }

    uint32_t codepoint = 0;
    error_type err = without_gil([byte, &codepoint] {
        return TextStringEncoding_PDFDocEncodingByteToUnicode(byte, &codepoint);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "TextStringEncoding_PDFDocEncodingByteToUnicode");
    }

    return to_python(codepoint);
}
