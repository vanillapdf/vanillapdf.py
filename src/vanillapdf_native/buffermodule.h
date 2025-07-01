#ifndef BUFFERMODULE_H
#define BUFFERMODULE_H

#include <Python.h>

PyObject* vp_buffer_create(PyObject* self, PyObject* args);
PyObject* vp_buffer_create_from_data(PyObject* self, PyObject* args);
PyObject* vp_buffer_set_data(PyObject* self, PyObject* args);
PyObject* vp_buffer_get_data(PyObject* self, PyObject* args);
PyObject* vp_buffer_release(PyObject* self, PyObject* args);

#endif // BUFFERMODULE_H
