#ifndef VANILLAPDF_PY_UTILS_LOGGING_H
#define VANILLAPDF_PY_UTILS_LOGGING_H

#include <Python.h>

PyObject* logging_get_severity(PyObject* self, PyObject* args);
PyObject* logging_set_severity(PyObject* self, PyObject* args);
PyObject* logging_set_pattern(PyObject* self, PyObject* args);
PyObject* logging_shutdown(PyObject* self, PyObject* args);

#endif // VANILLAPDF_PY_UTILS_LOGGING_H
