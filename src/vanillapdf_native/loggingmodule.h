#ifndef LOGGINGMODULE_H
#define LOGGINGMODULE_H

#include <Python.h>

PyObject* logging_get_severity(PyObject* self, PyObject* args);
PyObject* logging_set_severity(PyObject* self, PyObject* args);
PyObject* logging_set_pattern(PyObject* self, PyObject* args);
PyObject* logging_shutdown(PyObject* self, PyObject* args);

#endif // LOGGINGMODULE_H
