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

/* Python logger the native output is routed to. Named after the package, per
 * the usual `logging.getLogger(__name__)` convention, so it is discoverable and
 * configurable by consumers (logging.getLogger("vanillapdf")). Kept a fixed,
 * documented name rather than a configurable one -- Python logging already lets
 * callers re-route it via handlers/propagation without extra API. */
static const char* const NATIVE_LOGGER_NAME = "vanillapdf";

/* Cached logging.getLogger(NATIVE_LOGGER_NAME). Written exactly once, in
 * install_native_logging() at module import (single-threaded, before the sink
 * callback is even registered), and never mutated afterwards -- so the reads in
 * native_log_sink (which may run on any thread) need no synchronization. */
static PyObject* g_native_logger = nullptr;

static int severity_to_level(LoggingSeverity level) {
    switch (level) {
        case LoggingSeverity_Trace:
        case LoggingSeverity_Debug:
            return 10;  /* logging.DEBUG */
        case LoggingSeverity_Info:
            return 20;  /* logging.INFO */
        case LoggingSeverity_Warning:
            return 30;  /* logging.WARNING */
        case LoggingSeverity_Error:
            return 40;  /* logging.ERROR */
        case LoggingSeverity_Critical:
            return 50;  /* logging.CRITICAL */
        default:
            return 20;
    }
}

/* Native log sink -> Python logging. Runs synchronously inside a native log
 * call; acquire the GIL defensively and never let a Python exception escape
 * back into C++. */
static void CALLING_CONVENTION native_log_sink(
        void* /* user_data */, LoggingSeverity level, string_type payload, size_type length) {
    if (g_native_logger == nullptr) {
        return;  /* discard: still keeps native output off stdout */
    }

    PyGILState_STATE gil = PyGILState_Ensure();
    SCOPE_GUARD([gil] { PyGILState_Release(gil); });

    /* Trim the trailing newline the formatted payload usually carries. */
    Py_ssize_t size = static_cast<Py_ssize_t>(length);
    while (size > 0 && (payload[size - 1] == '\n' || payload[size - 1] == '\r')) {
        --size;
    }

    PyObject* message = PyUnicode_DecodeUTF8(payload, size, "replace");
    if (message == nullptr) {
        PyErr_Clear();  /* e.g. MemoryError -> drop the line, never raise into C++ */
        return;
    }
    SCOPE_GUARD([message] { Py_DECREF(message); });

    PyObject* result = PyObject_CallMethod(
        g_native_logger, "log", "iO", severity_to_level(level), message);
    Py_XDECREF(result);

    /* A logging handler failure must not propagate into the C++ caller. */
    if (PyErr_Occurred()) {
        PyErr_Clear();
    }
}

static void CALLING_CONVENTION native_log_flush(void* /* user_data */) {
}

int install_native_logging(void) {
    /* Cache logging.getLogger("vanillapdf"). If logging is unavailable we still
     * install the callback below, so native output never reaches stdout. */
    PyObject* logging_module = PyImport_ImportModule("logging");
    if (logging_module != nullptr) {
        SCOPE_GUARD([logging_module] { Py_DECREF(logging_module); });
        g_native_logger = PyObject_CallMethod(logging_module, "getLogger", "s", NATIVE_LOGGER_NAME);
    }
    if (g_native_logger == nullptr) {
        PyErr_Clear();  /* import or getLogger failed; discard messages, don't fail import */
    }

    error_type err = Logging_SetCallbackLogger(native_log_sink, native_log_flush, nullptr);
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return -1;
    }
    return 0;
}
