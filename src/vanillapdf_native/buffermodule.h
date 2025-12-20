#ifndef BUFFERMODULE_H
#define BUFFERMODULE_H

#include <Python.h>
#include <vanillapdf/utils/c_buffer.h>

/* Heap-allocated box that the capsule points to */
typedef struct {
    BufferHandle* handle;  /* NULL => released */
} BufferHandleBox;

/* Create a Python capsule from an existing BufferHandle */
PyObject* buffer_capsule_from_handle(BufferHandle* handle);

PyObject* buffer_create(PyObject* self, PyObject* args);
PyObject* buffer_create_from_data(PyObject* self, PyObject* args);
PyObject* buffer_set_data(PyObject* self, PyObject* args);
PyObject* buffer_get_data(PyObject* self, PyObject* args);
PyObject* buffer_release(PyObject* self, PyObject* args);

#endif // BUFFERMODULE_H
