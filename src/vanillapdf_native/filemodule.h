#ifndef FILEMODULE_H
#define FILEMODULE_H

#include <Python.h>

PyObject* file_open(PyObject* self, PyObject* args);
PyObject* file_initialize(PyObject* self, PyObject* args);
PyObject* file_get_version(PyObject* self, PyObject* args);
PyObject* file_get_filename(PyObject* self, PyObject* args);
PyObject* file_is_encrypted(PyObject* self, PyObject* args);
PyObject* file_set_encryption_password(PyObject* self, PyObject* args);
PyObject* file_release(PyObject* self, PyObject* args);

#endif // FILEMODULE_H
