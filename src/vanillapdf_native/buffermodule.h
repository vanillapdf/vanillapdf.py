#ifndef BUFFERMODULE_H
#define BUFFERMODULE_H

#include <Python.h>

PyObject* buffer_create(PyObject* self, PyObject* args);
PyObject* buffer_create_from_data(PyObject* self, PyObject* args);
PyObject* buffer_set_data(PyObject* self, PyObject* args);
PyObject* buffer_get_data(PyObject* self, PyObject* args);
PyObject* buffer_release(PyObject* self, PyObject* args);

#endif // BUFFERMODULE_H
