#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_errors.h>

#include "utils/errors.h"
#include "common.h"

PyObject* errors_get_last_error(PyObject* self, PyObject* args) {
    error_type code = VANILLAPDF_ERROR_SUCCESS;
    error_type err = Errors_GetLastError(&code);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Errors_GetLastError");
    }

    return to_python(code);
}

PyObject* errors_get_printable_error_text(PyObject* self, PyObject* args) {
    unsigned long code_value;
    if (!PyArg_ParseTuple(args, "k", &code_value)) {
        return nullptr;
    }

    /* 'k' yields unsigned long (64-bit on LP64) while error_type is uint32_t;
     * narrow_unsigned rejects out-of-range values instead of truncating them
     * into a different, valid-looking error code. */
    error_type value = 0;
    if (safe_convert(code_value, &value) < 0) {
        return nullptr;
    }

    size_type size = 0;
    error_type length_err = Errors_GetPrintableErrorTextLength(value, &size);
    if (length_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(length_err, "Errors_GetPrintableErrorTextLength");
    }

    if (size == 0) {
        return PyUnicode_FromString("");
    }

    char* data = py_malloc<char>(size);
    if (data == nullptr) {
        return PyErr_NoMemory();
    }
    SCOPE_GUARD([data] { PyMem_Free(data); });

    error_type text_err = Errors_GetPrintableErrorText(value, data, size);
    if (text_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(text_err, "Errors_GetPrintableErrorText");
    }

    /* Length includes the trailing NUL character. */
    return PyUnicode_FromString(data);
}

PyObject* errors_get_last_error_message(PyObject* self, PyObject* args) {
    size_type size = 0;
    error_type length_err = Errors_GetLastErrorMessageLength(&size);
    if (length_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(length_err, "Errors_GetLastErrorMessageLength");
    }

    if (size <= 1) {
        /* Only the trailing NUL: no detail message. */
        return PyUnicode_FromString("");
    }

    char* data = py_malloc<char>(size);
    if (data == nullptr) {
        return PyErr_NoMemory();
    }
    SCOPE_GUARD([data] { PyMem_Free(data); });

    error_type message_err = Errors_GetLastErrorMessage(data, size);
    if (message_err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(message_err, "Errors_GetLastErrorMessage");
    }

    return PyUnicode_FromString(data);
}
