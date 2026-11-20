#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <vanillapdf/c_values.h>
#include <vanillapdf/utils/c_logging.h>

#include "utils/logging.h"
#include "common.h"

/* Discarding log sink. Its only job is to REPLACE the library's default stdout
 * sink at import (and again after Logging.shutdown(), see logging_shutdown) so
 * native output never reaches stdout: when stdout (fd 1) is
 * redirected and juggled with dup2 -- exactly what pytest's --capture=fd does --
 * the stdout sink's stale handle can write log text into the file the library is
 * saving, corrupting it.
 *
 * The sink is deliberately a no-op that never touches the Python C-API. The
 * native library invokes it synchronously while holding its internal log mutex;
 * a sink that acquired the GIL there would dead-lock against a thread holding the
 * GIL while blocked on that same mutex, now that native calls release the GIL
 * for parallelism (see `without_gil` in common.h). Callers who want native logs
 * opt into a real destination via Logging.set_rotating_file(), which installs
 * the native rotating-file sink -- streamed to disk (so it scales to heavy debug
 * output) and entirely native (no GIL, no dead-lock). */
static void CALLING_CONVENTION native_log_sink(
        void* /* user_data */, LoggingSeverity /* level */,
        string_type /* payload */, size_type /* length */) {
    /* discard */
}

static void CALLING_CONVENTION native_log_flush(void* /* user_data */) {
}

PyObject* logging_get_severity(PyObject* self, PyObject* args) {
    LoggingSeverity severity;
    error_type err = without_gil([&severity] {
        return Logging_GetSeverity(&severity);
    });
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
    error_type err = without_gil([level] {
        return Logging_SetSeverity(level);
    });
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

    error_type err = without_gil([pattern] {
        return Logging_SetPattern(pattern);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Logging_SetPattern");
    }

    Py_RETURN_NONE;
}

PyObject* logging_shutdown(PyObject* self, PyObject* args) {
    /* WORKAROUND for https://github.com/vanillapdf/vanillapdf/issues/612;
     * remove once the library keeps its default logger alive across
     * Logging_Shutdown.
     *
     * Logging_Shutdown runs spdlog::shutdown(), which drops every logger, the
     * default one included. The library logs through the default logger without
     * checking it, so the next native call that logs would dereference null and
     * crash. Reinstall the discarding sink straight away: shutdown then flushes
     * and releases the current sink (e.g. closes the rotating log file) and the
     * library stays usable. A thread that logs between the two calls can still
     * crash; only the native fix closes that window. */
    const char* failed_operation = "Logging_Shutdown";
    error_type err = without_gil([&failed_operation] {
        error_type result = Logging_Shutdown();
        if (result != VANILLAPDF_ERROR_SUCCESS) {
            return result;
        }

        failed_operation = "Logging_SetCallbackLogger";
        return Logging_SetCallbackLogger(native_log_sink, native_log_flush, nullptr);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, failed_operation);
    }

    Py_RETURN_NONE;
}

PyObject* logging_set_rotating_file(PyObject* self, PyObject* args) {
    const char* filename;
    int max_file_size;
    int max_files;
    if (!PyArg_ParseTuple(args, "sii", &filename, &max_file_size, &max_files)) {
        return nullptr;
    }

    error_type err = without_gil([filename, max_file_size, max_files] {
        return Logging_SetRotatingFileLogger(filename, max_file_size, max_files);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return raise_last_error(err, "Logging_SetRotatingFileLogger");
    }

    Py_RETURN_NONE;
}

int install_native_logging(void) {
    error_type err = without_gil([] {
        return Logging_SetCallbackLogger(native_log_sink, native_log_flush, nullptr);
    });
    if (err != VANILLAPDF_ERROR_SUCCESS) {
        return -1;
    }
    return 0;
}
