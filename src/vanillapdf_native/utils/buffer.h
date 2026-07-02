#ifndef VANILLAPDF_PY_UTILS_BUFFER_H
#define VANILLAPDF_PY_UTILS_BUFFER_H

#include <Python.h>
#include <vanillapdf/utils/c_buffer.h>

/* Wrap an existing BufferHandle in a capsule (takes ownership). Public so
 * other modules can return Buffers produced by the library. */
PyObject* buffer_capsule_from_handle(BufferHandle* handle);

PyObject* buffer_create(PyObject* self, PyObject* args);
PyObject* buffer_create_from_data(PyObject* self, PyObject* args);
PyObject* buffer_set_data(PyObject* self, PyObject* args);
PyObject* buffer_get_data(PyObject* self, PyObject* args);
PyObject* buffer_release(PyObject* self, PyObject* args);

#endif // VANILLAPDF_PY_UTILS_BUFFER_H
