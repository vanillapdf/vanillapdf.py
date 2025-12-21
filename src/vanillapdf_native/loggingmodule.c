#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_logging.h>

#include "loggingmodule.h"

PyObject* logging_get_severity(PyObject* self, PyObject* args) {
    LoggingSeverity severity;
    error_type err = Logging_GetSeverity(&severity);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to get logging severity");
        return NULL;
    }
    return PyLong_FromLong((long)severity);
}

PyObject* logging_set_severity(PyObject* self, PyObject* args) {
    int severity;
    if (!PyArg_ParseTuple(args, "i", &severity)) {
        return NULL;
    }

    error_type err = Logging_SetSeverity((LoggingSeverity)severity);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to set logging severity");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* logging_set_pattern(PyObject* self, PyObject* args) {
    const char* pattern;
    if (!PyArg_ParseTuple(args, "s", &pattern)) {
        return NULL;
    }

    error_type err = Logging_SetPattern(pattern);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to set logging pattern");
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject* logging_shutdown(PyObject* self, PyObject* args) {
    error_type err = Logging_Shutdown();
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to shutdown logging");
        return NULL;
    }

    Py_RETURN_NONE;
}
