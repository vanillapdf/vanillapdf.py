#ifndef VANILLAPDF_PY_SYNTAX_STREAM_OBJECT_H
#define VANILLAPDF_PY_SYNTAX_STREAM_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* Stream object. Header is a Dictionary "VanillaPDF.Object"; body is a Buffer. */
PyObject* stream_object_create(PyObject* self, PyObject* args);
PyObject* stream_object_get_header(PyObject* self, PyObject* args);
PyObject* stream_object_set_header(PyObject* self, PyObject* args);
PyObject* stream_object_get_body(PyObject* self, PyObject* args);
PyObject* stream_object_get_body_raw(PyObject* self, PyObject* args);
PyObject* stream_object_set_body(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SYNTAX_STREAM_OBJECT_H */
