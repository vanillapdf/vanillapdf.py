#ifndef VANILLAPDF_PY_SYNTAX_ARRAY_OBJECT_H
#define VANILLAPDF_PY_SYNTAX_ARRAY_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* Array object. All functions take/return a "VanillaPDF.Object" capsule. */
PyObject* array_object_create(PyObject* self, PyObject* args);
PyObject* array_object_get_size(PyObject* self, PyObject* args);
PyObject* array_object_get_value(PyObject* self, PyObject* args);
PyObject* array_object_set_value(PyObject* self, PyObject* args);
PyObject* array_object_append(PyObject* self, PyObject* args);
PyObject* array_object_insert(PyObject* self, PyObject* args);
PyObject* array_object_remove(PyObject* self, PyObject* args);
PyObject* array_object_clear(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SYNTAX_ARRAY_OBJECT_H */
