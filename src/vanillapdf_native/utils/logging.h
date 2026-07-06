#ifndef VANILLAPDF_PY_UTILS_LOGGING_H
#define VANILLAPDF_PY_UTILS_LOGGING_H

#include <Python.h>

PyObject* logging_get_severity(PyObject* self, PyObject* args);
PyObject* logging_set_severity(PyObject* self, PyObject* args);
PyObject* logging_set_pattern(PyObject* self, PyObject* args);
PyObject* logging_set_rotating_file(PyObject* self, PyObject* args);
PyObject* logging_shutdown(PyObject* self, PyObject* args);

/* Replace the native library's default stdout log sink with a discarding one,
 * so native output never reaches stdout. Called once at module init: the library
 * defaults to an stdout sink at INFO, and under fd redirection (pytest's
 * --capture=fd) that corrupts saved files, so we must replace it before any
 * native call. Callers opt into real native logs via Logging.set_rotating_file. */
int install_native_logging(void);

#endif // VANILLAPDF_PY_UTILS_LOGGING_H
