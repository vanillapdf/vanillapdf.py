#ifndef VANILLAPDF_PY_UTILS_LOGGING_H
#define VANILLAPDF_PY_UTILS_LOGGING_H

#include <Python.h>

PyObject* logging_get_severity(PyObject* self, PyObject* args);
PyObject* logging_set_severity(PyObject* self, PyObject* args);
PyObject* logging_set_pattern(PyObject* self, PyObject* args);
PyObject* logging_shutdown(PyObject* self, PyObject* args);

/* Route the native library's logging into Python's `logging` module via a
 * callback sink, so it never writes to stdout. Called once at module init.
 * The library defaults to an stdout sink at INFO; under fd redirection that
 * corrupts saved files, so we must replace it before any native call. */
int install_native_logging(void);

#endif // VANILLAPDF_PY_UTILS_LOGGING_H
