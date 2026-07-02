#ifndef VANILLAPDF_PY_SYNTAX_INTEGER_OBJECT_H
#define VANILLAPDF_PY_SYNTAX_INTEGER_OBJECT_H

#define PY_SSIZE_T_CLEAN

#include <Python.h>

/* integer_object_create(value: int) -> Object (signed 64-bit) */
PyObject* integer_object_create(PyObject* self, PyObject* args);
/* integer_object_get_value(Object) -> int */
PyObject* integer_object_get_value(PyObject* self, PyObject* args);
/* integer_object_set_value(Object, value: int) -> None */
PyObject* integer_object_set_value(PyObject* self, PyObject* args);

#endif /* VANILLAPDF_PY_SYNTAX_INTEGER_OBJECT_H */
