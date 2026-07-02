#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_logging.h>

#include "utils/logging.h"
#include "common.h"

PyObject* logging_get_severity(PyObject* self, PyObject* args) {
    LoggingSeverity severity;
    error_type err = Logging_GetSeverity(&severity);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Logging_GetSeverity");
    }
    return to_python(severity);
}

PyObject* logging_set_severity(PyObject* self, PyObject* args) {
    int severity;
    if (!PyArg_ParseTuple(args, "i", &severity)) {
        return nullptr;
    }

    LoggingSeverity level = static_cast<LoggingSeverity>(severity);
    error_type err = Logging_SetSeverity(level);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Logging_SetSeverity");
    }

    Py_RETURN_NONE;
}

PyObject* logging_set_pattern(PyObject* self, PyObject* args) {
    const char* pattern;
    if (!PyArg_ParseTuple(args, "s", &pattern)) {
        return nullptr;
    }

    error_type err = Logging_SetPattern(pattern);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Logging_SetPattern");
    }

    Py_RETURN_NONE;
}

PyObject* logging_shutdown(PyObject* self, PyObject* args) {
    error_type err = Logging_Shutdown();
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Logging_Shutdown");
    }

    Py_RETURN_NONE;
}
