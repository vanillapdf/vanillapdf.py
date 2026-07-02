#ifndef VANILLAPDF_PY_ERRORSMODULE_H
#define VANILLAPDF_PY_ERRORSMODULE_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* Errors_GetLastError() -> int (thread-local last error code) */
PyObject* errors_get_last_error(PyObject* self, PyObject* args);

/* Errors_GetPrintableErrorText(code) -> str (human readable error name) */
PyObject* errors_get_printable_error_text(PyObject* self, PyObject* args);

/* Errors_GetLastErrorMessage() -> str (thread-local detail message) */
PyObject* errors_get_last_error_message(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_ERRORSMODULE_H */
